from migen import *
from migen.genlib.cdc import MultiReg

from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *


class btnintrupt(Module, AutoCSR):
    def __init__(self, signal):
        self._in = CSRStatus(len(signal))
        self._dir= CSRStatus(8);
        self.specials += MultiReg(signal, self._in.status)

        self.submodules.ev = EventManager()
        self.ev.arriba = EventSourceProcess()
        self.ev.abajo = EventSourceProcess()
        self.ev.derecha = EventSourceProcess()
        self.ev.izquierda = EventSourceProcess()
        self.ev.pause = EventSourceProcess()


        self.comb +=[
            self.ev.arriba.trigger.eq(~signal[1]),
            self.ev.abajo.trigger.eq(~signal [0]),
            self.ev.derecha.trigger.eq(~signal[2]),
            self.ev.izquierda.trigger.eq(~signal[3]),
            self.ev.pause.trigger.eq(~signal[4])
        ]
