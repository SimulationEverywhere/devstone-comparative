from abc import abstractmethod
from unittest import TestCase
import random

from pypdevs.DEVS import AtomicDEVS, CoupledDEVS
from pypdevs.simulator import Simulator

from devstone import LI, DelayedAtomic, DelayedAtomicStats, HI, HO
from generator import Generator
from main import DEVStoneEnvironment


class Utils:

    @staticmethod
    def count_atomics(coupled):
        """
        :return: Number of atomic components in a coupled model
        """
        atomic_count = 0
        for comp in coupled.component_set:
            if isinstance(comp, AtomicDEVS):
                atomic_count += 1
            elif isinstance(comp, CoupledDEVS):
                atomic_count += Utils.count_atomics(comp)
            else:
                raise RuntimeError("Unrecognized type of component")

        return atomic_count

    @staticmethod
    def count_ic(coupled):
        """
        :return: Number of ic couplings in a coupled model
        """
        ic_count = 0
        for comp in coupled.component_set:
            for src_port in comp.OPorts:
                for dst_port in src_port.outline:
                    if dst_port.host_DEVS != coupled:
                        ic_count += 1

        for comp in coupled.component_set:
            if isinstance(comp, CoupledDEVS):
                ic_count += Utils.count_ic(comp)
            elif not isinstance(comp, AtomicDEVS):
                raise RuntimeError("Unrecognized type of component")

        return ic_count

    @staticmethod
    def count_eic(coupled):
        """
        :return: Number of eic couplings in a coupled model
        """
        eic_count = sum([len(port.outline) for port in coupled.IPorts])
        for comp in coupled.component_set:
            if isinstance(comp, CoupledDEVS):
                eic_count += Utils.count_eic(comp)
            elif not isinstance(comp, AtomicDEVS):
                raise RuntimeError("Unrecognized type of component")

        return eic_count

    @staticmethod
    def count_eoc(coupled):
        """
        :return: Number of eoc couplings in a coupled model
        """
        eoc_count = sum([len(port.inline) for port in coupled.OPorts])
        for comp in coupled.component_set:
            if isinstance(comp, CoupledDEVS):
                eoc_count += Utils.count_eoc(comp)
            elif not isinstance(comp, AtomicDEVS):
                raise RuntimeError("Unrecognized type of component")

        return eoc_count

    @staticmethod
    def count_transitions(coupled):
        """
        :return: Number of atomic components in a coupled model
        """
        int_count = 0
        ext_count = 0
        for comp in coupled.component_set:
            if isinstance(comp, DelayedAtomicStats):
                int_count += comp.int_count
                ext_count += comp.ext_count
            elif isinstance(comp, CoupledDEVS):
                pic, pec = Utils.count_transitions(comp)
                int_count += pic
                ext_count += pec
            elif not isinstance(comp, Generator):
                raise RuntimeError("Unexpected type of component: %s" % comp.__class__.__name__)

        return int_count, ext_count


class DevstoneUtilsTestCase(TestCase):

    def __init__(self, name, num_valid_params_sets: int = 10):
        super().__init__(name)
        self.valid_high_params = []
        self.valid_low_params = []

        for _ in range(int(num_valid_params_sets)):
            self.valid_high_params.append([random.randint(1, 100), random.randint(1, 200),
                                           random.randint(1, 1000), random.randint(1, 1000)])

        for _ in range(int(num_valid_params_sets)):
            self.valid_low_params.append([random.randint(1, 20), random.randint(1, 30),
                                      random.randint(0, 10), random.randint(0, 10)])

    def check_invalid_inputs(self, base_class):
        self.assertRaises(ValueError, base_class, "root", 0, 1, 1, 1)
        self.assertRaises(ValueError, base_class, "root", 1, 0, 1, 1)
        self.assertRaises(ValueError, base_class, "root", 1, 1, -1, 0)
        self.assertRaises(ValueError, base_class, "root", 1, 1, 0, -1)
        self.assertRaises(ValueError, base_class, "root", 0, 1, -1, -1)
        self.assertRaises(ValueError, base_class, "root", 0, 0, -1, -1)

    def test_behavior_sequential(self):
        self._test_behavior(Simulator)

    #def test_behavior_parallel(self):
    #    self._test_behavior(ParallelThreadCoordinator)

    @abstractmethod
    def _test_behavior(self, Coordinator):
        pass


class TestLI(DevstoneUtilsTestCase):

    def test_structure(self):
        """
        Check structure params: atomic modules, ic's, eic's and eoc's.
        """
        for params_tuple in self.valid_high_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                self._check_structure(**params)

    def test_structure_corner_cases(self):
        params = {"depth": 10, "width": 1, "int_delay": 1, "ext_delay": 1}
        self._check_structure(**params)
        params["depth"] = 1
        self._check_structure(**params)

    def _check_structure(self, **params):
        li_root = LI("LI_root", **params)
        self.assertEqual(Utils.count_atomics(li_root), (params["width"] - 1) * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eic(li_root), params["width"] * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eoc(li_root), params["depth"])
        self.assertEqual(Utils.count_ic(li_root), 0)

    def _test_behavior(self, coord_type):
        """
        Check behaviour params: number of int and ext transitions.
        """
        for params_tuple in self.valid_low_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                li_root = LI("LI_root", stats=True, **params)
                li_env = DEVStoneEnvironment("LI_env", li_root)
                sim = Simulator(li_env)
                sim.setVerbose(None)
                # sim.setTerminationTime(10.0)
                # sim.setStateSaving("custom")
                sim.simulate()

                int_count, ext_count = Utils.count_transitions(li_root)
                self.assertEqual(int_count, (params["width"] - 1) * (params["depth"] - 1) + 1)
                self.assertEqual(ext_count, (params["width"] - 1) * (params["depth"] - 1) + 1)

    def test_invalid_inputs(self):
        super().check_invalid_inputs(LI)


class TestHI(DevstoneUtilsTestCase):

    def test_structure(self):
        """
        Check structure params: atomic modules, ic's, eic's and eoc's.
        """
        for params_tuple in self.valid_high_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                self._check_structure(**params)

    def test_structure_corner_cases(self):
        params = {"depth": 10, "width": 1, "int_delay": 1, "ext_delay": 1}
        self._check_structure(**params)
        params["depth"] = 1
        self._check_structure(**params)

    def _check_structure(self, **params):
        hi_root = HI("HI_root", **params)
        self.assertEqual(Utils.count_atomics(hi_root), (params["width"] - 1) * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eic(hi_root), params["width"] * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eoc(hi_root), params["depth"])
        self.assertEqual(Utils.count_ic(hi_root),
                         (params["width"] - 2) * (params["depth"] - 1) if params["width"] > 2 else 0)

    def _test_behavior(self, coord_type):
        """
        Check behaviour params: number of int and ext transitions.
        """
        for params_tuple in self.valid_low_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                hi_root = HI("HI_root", stats=True, **params)
                hi_env = DEVStoneEnvironment("HI_env", hi_root)
                sim = Simulator(hi_env)
                sim.setVerbose(None)
                # sim.setTerminationTime(10.0)
                # sim.setStateSaving("custom")
                sim.simulate()

                int_count, ext_count = Utils.count_transitions(hi_root)
                self.assertEqual(int_count, (((params["width"] - 1) * params["width"]) / 2) * (params["depth"] - 1) + 1)
                self.assertEqual(ext_count, (((params["width"] - 1) * params["width"]) / 2) * (params["depth"] - 1) + 1)

    def test_invalid_inputs(self):
        super().check_invalid_inputs(HI)


class TestHO(DevstoneUtilsTestCase):

    def test_structure(self):
        """
        Check structure params: atomic modules, ic's, eic's and eoc's.
        """
        for params_tuple in self.valid_high_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                self._check_structure(**params)

    def test_structure_corner_cases(self):
        params = {"depth": 10, "width": 1, "int_delay": 1, "ext_delay": 1}
        self._check_structure(**params)
        params["depth"] = 1
        self._check_structure(**params)

    def _check_structure(self, **params):
        ho_root = HO("HO_root", **params)
        self.assertEqual(Utils.count_atomics(ho_root), (params["width"] - 1) * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eic(ho_root), (params["width"] + 1) * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_eoc(ho_root), params["width"] * (params["depth"] - 1) + 1)
        self.assertEqual(Utils.count_ic(ho_root),
                         (params["width"] - 2) * (params["depth"] - 1) if params["width"] > 2 else 0)

    def _test_behavior(self, coord_type):
        """
        Check behaviour params: number of int and ext transitions.
        """
        for params_tuple in self.valid_low_params:
            params = dict(zip(("depth", "width", "int_delay", "ext_delay"), params_tuple))

            with self.subTest(**params):
                ho_root = HO("HO_root", stats=True, **params)
                ho_env = DEVStoneEnvironment("HO_env", ho_root)
                sim = Simulator(ho_env)
                sim.setVerbose(None)
                # sim.setTerminationTime(10.0)
                # sim.setStateSaving("custom")
                sim.simulate()

                int_count, ext_count = Utils.count_transitions(ho_env)
                self.assertEqual(int_count, (((params["width"] - 1) * params["width"]) / 2) * (params["depth"] - 1) + 1)
                self.assertEqual(ext_count, (((params["width"] - 1) * params["width"]) / 2) * (params["depth"] - 1) + 1)

    def test_invalid_inputs(self):
        super().check_invalid_inputs(HO)