from typing import Any

from pypdevs.minimal import AtomicDEVS
from pypdevs.infinity import INFINITY


class Generator(AtomicDEVS):

    def __init__(self, name, num_outputs=1, period=None):
        super(Generator, self).__init__(name=name)
        self.num_outputs = num_outputs
        self.period = period
        self.state = "active"
        self.o_out = self.addOutPort("o_out")

    def intTransition(self):
        if self.state == "active" and self.period is not None:
            self.state = "waiting"
        elif self.state == "waiting":
            self.state = "active"
        else:
            self.state = "passive"

    def timeAdvance(self):
        if self.state == "active":
            return 0
        elif self.state == "waiting":
            return self.period
        else:
            return INFINITY

    def outputFnc(self):
        return {self.o_out: list(range(self.num_outputs))}

    def extTransition(self, inputs):
        pass
