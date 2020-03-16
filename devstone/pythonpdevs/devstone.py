from abc import ABC, abstractmethod
from collections import defaultdict

from pystone import pystones
from pypdevs.DEVS import AtomicDEVS, CoupledDEVS
from pypdevs.infinity import INFINITY
from pypdevs.simulator import Simulator


class DelayedAtomic(AtomicDEVS):
    def __init__(self, name: str, int_delay: float, ext_delay: float, add_out_port: bool = False, prep_time=0):
        super().__init__(name)

        self.int_delay = int_delay
        self.ext_delay = ext_delay
        self.prep_time = prep_time

        self.i_in = self.addInPort("i_in")

        if add_out_port:
            self.o_out = self.addOutPort("o_out")

    def intTransition(self):
        if self.int_delay:
            pystones(self.int_delay)

        return "passive"

    def timeAdvance(self):
        if self.state == "active":
            return self.prep_time
        else:
            return INFINITY

    def outputFnc(self):
        if hasattr(self, "o_out"):
            return {self.o_out: [0]}
        return {}

    def extTransition(self, inputs):
        if self.ext_delay:
            pystones(self.ext_delay)

        return "active"


class DelayedAtomicStats(DelayedAtomic):
    def __init__(self, name: str, int_delay: float, ext_delay: float, add_out_port: bool = False, prep_time=0):
        super().__init__(name, int_delay, ext_delay, add_out_port, prep_time)

        self.int_count = 0
        self.ext_count = 0

    def intTransition(self):
        self.int_count += 1
        return super().intTransition()

    def extTransition(self, inputs):
        self.ext_count += 1
        return super().extTransition(inputs)


class DEVStoneWrapper(CoupledDEVS, ABC):

    def __init__(self, name: str, depth: int, width: int, int_delay: float, ext_delay: float,
                 add_atomic_out_ports: bool = False, prep_time=0, stats=False):
        super().__init__(name)

        self.depth = depth
        self.width = width
        self.int_delay = int_delay
        self.ext_delay = ext_delay
        self.prep_time = prep_time
        self.stats = stats
        self.add_atomic_out_ports = add_atomic_out_ports

        self.i_in = self.addInPort("i_in")
        self.o_out = self.addOutPort("o_out")

        if depth < 1:
            raise ValueError("Invalid depth")
        if width < 1:
            raise ValueError("Invalid width")
        if int_delay < 0:
            raise ValueError("Invalid int_delay")
        if ext_delay < 0:
            raise ValueError("Invalid ext_delay")

        if depth == 1:
            if self.stats:
                atomic = DelayedAtomicStats("Atomic_0_0", int_delay, ext_delay, add_out_port=True, prep_time=prep_time)
            else:
                atomic = DelayedAtomic("Atomic_0_0", int_delay, ext_delay, add_out_port=True, prep_time=prep_time)

            self.addSubModel(atomic)

            self.connectPorts(self.i_in, atomic.i_in)
            self.connectPorts(atomic.o_out, self.o_out)
        else:
            coupled = self.gen_coupled()
            self.addSubModel(coupled)
            self.connectPorts(self.i_in, coupled.i_in)
            self.connectPorts(coupled.o_out, self.o_out)

            for idx in range(width - 1):
                if self.stats:
                    atomic = DelayedAtomicStats("Atomic_%d_%d" % (depth - 1, idx), int_delay, ext_delay,
                                                add_out_port=add_atomic_out_ports, prep_time=prep_time)
                else:
                    atomic = DelayedAtomic("Atomic_%d_%d" % (depth - 1, idx), int_delay, ext_delay,
                                           add_out_port=add_atomic_out_ports, prep_time=prep_time)
                self.addSubModel(atomic)

    @abstractmethod
    def gen_coupled(self):
        """ :return a coupled method with i_in and o_out ports"""
        pass


class LI(DEVStoneWrapper):

    def __init__(self, name: str, depth: int, width: int, int_delay: float, ext_delay: float, prep_time=0, stats=False):
        super().__init__(name, depth, width, int_delay, ext_delay, add_atomic_out_ports=False, prep_time=prep_time,
                         stats=stats)

        for idx in range(1, len(self.component_set)):
            assert isinstance(self.component_set[idx], AtomicDEVS)
            self.connectPorts(self.i_in, self.component_set[idx].i_in)

    def gen_coupled(self):
        return LI("Coupled_%d" % (self.depth - 1), self.depth - 1, self.width, self.int_delay, self.ext_delay,
                  prep_time=self.prep_time, stats=self.stats)


class HI(DEVStoneWrapper):

    def __init__(self, name: str, depth: int, width: int, int_delay: float, ext_delay: float, prep_time=0, stats=False):
        super().__init__(name, depth, width, int_delay, ext_delay, add_atomic_out_ports=True, prep_time=prep_time,
                         stats=stats)

        if len(self.component_set) > 1:
            assert isinstance(self.component_set[-1], AtomicDEVS)
            self.connectPorts(self.i_in, self.component_set[-1].i_in)

        for idx in range(1, len(self.component_set) - 1):
            assert isinstance(self.component_set[idx], AtomicDEVS)
            self.connectPorts(self.component_set[idx].o_out, self.component_set[idx + 1].i_in)
            self.connectPorts(self.i_in, self.component_set[idx].i_in)

    def gen_coupled(self):
        return HI("Coupled_%d" % (self.depth - 1), self.depth - 1, self.width, self.int_delay, self.ext_delay,
                  prep_time=self.prep_time, stats=self.stats)


class HO(DEVStoneWrapper):

    def __init__(self, name: str, depth: int, width: int, int_delay: float, ext_delay: float, prep_time=0, stats=False):
        super().__init__(name, depth, width, int_delay, ext_delay, add_atomic_out_ports=True, prep_time=prep_time,
                         stats=stats)

        self.i_in2 = self.addInPort("i_in2")
        self.o_out2 = self.addOutPort("o_out2")

        assert len(self.component_set) > 0
        if isinstance(self.component_set[0], CoupledDEVS):
            self.connectPorts(self.i_in, self.component_set[0].i_in2)

        if len(self.component_set) > 1:
            assert isinstance(self.component_set[-1], AtomicDEVS)
            self.connectPorts(self.i_in2, self.component_set[-1].i_in)
            self.connectPorts(self.component_set[-1].o_out, self.o_out2)

        for idx in range(1, len(self.component_set) - 1):
            assert isinstance(self.component_set[idx], AtomicDEVS)
            self.connectPorts(self.component_set[idx].o_out, self.component_set[idx + 1].i_in)
            self.connectPorts(self.i_in2, self.component_set[idx].i_in)
            self.connectPorts(self.component_set[idx].o_out, self.o_out2)

    def gen_coupled(self):
        return HO("Coupled_%d" % (self.depth - 1), self.depth - 1, self.width, self.int_delay, self.ext_delay,
                  prep_time=self.prep_time, stats=self.stats)


class HOmod(CoupledDEVS):

    def __init__(self, name: str, depth: int, width: int, int_delay: float, ext_delay: float):
        super().__init__(name)

        self.depth = depth
        self.width = width
        self.int_delay = int_delay
        self.ext_delay = ext_delay

        self.i_in = self.addInPort("i_in")
        self.i_in2 = self.addInPort("i_in2")
        self.o_out = self.addOutPort("o_out")

        if depth < 1:
            raise ValueError("Invalid depth")
        if width < 1:
            raise ValueError("Invalid width")
        if int_delay < 0:
            raise ValueError("Invalid int_delay")
        if ext_delay < 0:
            raise ValueError("Invalid ext_delay")

        if depth == 1:
            atomic = DelayedAtomic("Atomic_0_0", int_delay, ext_delay, add_out_port=True)
            self.addSubModel(atomic)

            self.connectPorts(self.i_in, atomic.i_in)
            self.connectPorts(atomic.o_out, self.o_out)
        else:
            coupled = HOmod("Coupled_%d" % (self.depth - 1), self.depth - 1, self.width, self.int_delay, self.ext_delay)
            self.addSubModel(coupled)
            self.connectPorts(self.i_in, coupled.i_in)
            self.connectPorts(coupled.o_out, self.o_out)

            if width >= 2:
                atomics = defaultdict(list)

                # Generate atomic components
                for i in range(width):
                    min_row_idx = 0 if i < 2 else i - 1
                    for j in range(min_row_idx, width - 1):
                        atomic = DelayedAtomic("Atomic_%d_%d_%d" % (depth - 1, i, j), int_delay, ext_delay,
                                               add_out_port=True)
                        self.addSubModel(atomic)
                        atomics[i].append(atomic)

                # Connect EIC
                for atomic in atomics[0]:
                    self.connectPorts(self.i_in2, atomic.i_in)
                for i in range(1, width):
                    atomic_set = atomics[i]
                    self.connectPorts(self.i_in2, atomic_set[0].i_in)

                # Connect IC
                for atomic in atomics[0]:  # First row to coupled component
                    self.connectPorts(atomic.o_out, coupled.i_in2)
                for i in range(len(atomics[1])):  # Second to first rows
                    self.connectPorts(atomics[1][i].o_out, atomics[0][i].i_in)
                for i in range(2, width):  # Rest of rows
                    for j in range(len(atomics[i])):
                        self.connectPorts(atomics[i][j].o_out, atomics[i - 1][j + 1].i_in)


if __name__ == '__main__':
    import sys

    sys.setrecursionlimit(10000)
    root = HOmod("Root", 4, 3, 0, 0)
    sim = Simulator(root)
    sim.setVerbose(None)
    # sim.setTerminationTime(10.0)
    sim.setStateSaving("custom")
    sim.simulate()
