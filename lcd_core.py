from migen import *
from litex.soc.interconnect.csr import *

class _LCD_i80(Module, AutoCSR):
    def __init__(self,clk):

        self.data=data=Signal(8)
        self.addr=addr=Signal(8)
        self.busy=busy=Signal()
        self.start=start=Signal()
        self.option=option=Signal()

        self.db=Signal(8)
        self.rs=Signal()
        self.rd=Signal()
        self.wr=Signal()
        self.rst=Signal()

        manage=Signal(4)
        count2=Signal(4)
        countwr=Signal(4)
        manage_ant=Signal(3)

        fsm=FSM(reset_state="START")
        self.submodules+=fsm

        fsm.act("START",
            NextValue(self.rs,1),
            NextValue(self.rd,1),
            NextValue(self.wr,1),
            NextValue(self.db,0x00),
            NextValue(busy,0),
            NextValue(count2,0x0),
            NextValue(self.rst,1),
            NextValue(manage,0x0),
            If(start==0b01,NextState("MANAGER"))
        )
        fsm.act("SETADDR",
            NextValue(self.rs,0),
            NextValue(self.db,addr),
            NextValue(busy,1),
            NextValue(manage,0x1),
            NextValue(manage_ant,0x1),
            NextValue(count2,0x0),
            NextState("MANAGER")
        )
        fsm.act("WRSTROBE",
            NextValue(busy,1),
            NextValue(self.wr,~self.wr),
            NextValue(manage,0x2),
            NextValue(countwr,0x0),
            NextValue(count2,0x0),
            If(self.wr==1,NextState("MANAGERWR")),
            If(self.wr==0,NextState("MANAGER"))
        )
        fsm.act("Data",
            NextValue(busy,1),
            NextValue(self.rs,1),
            NextValue(self.db,data),
            NextValue(manage,0x3),
            NextValue(manage_ant,0x3),
            NextValue(count2,0x0),
            NextState("MANAGER")
        )
        
        fsm.act("MANAGER",
            NextValue(count2,count2+1),
            If(count2<10*int((clk/100e6)+1),NextState("MANAGER")),
            If(count2>=10*int((clk/100e6)+1),
                If(manage==0x0,
                    If(option==0x1,NextState("Data")),
                    If(option==0x0,NextState("SETADDR")),
                ),
                If(manage==0x1,NextState("WRSTROBE")),              
                If(manage==0x2,
                    If(manage_ant==0x1,
                        If(option==1,NextState("Data")),
                        If(option==0,NextState("START")),
                    ),
                    If(manage_ant==0x3,NextState("START")),
                    If(manage_ant==0x4,NextState("WAIT_START")),
                ),
                If(manage==0x3,NextState("WRSTROBE")),
                If(manage==0x4,NextState("WRSTROBE")),
                If(manage==0x5,NextState("WRSTROBE")),
            ),
        )
        fsm.act("WAIT_START",
            NextValue(countwr,countwr+1),
            If(countwr<3*int((clk/100e6)+1),NextState("WAIT_START")),
            If(countwr>=3*int((clk/100e6)+1),
                NextState("START"),
            )
        )
        fsm.act("MANAGERWR",
            NextValue(countwr,countwr+1),
            If(countwr<10*int((clk/100e6)+1),NextState("MANAGERWR")),
            If(countwr>=10*int((clk/100e6)+1),
                NextState("WRSTROBE"),
            )
        )
class LCD_i80(Module, AutoCSR):
    def __init__(self,clk):

        # # # Control Registers
        self.DATA=CSRStorage(8)
        self.ADDR=CSRStorage(8)
        self.BUSY=CSRStatus()
        self.START=CSRStorage(2)
        self.OPTION=CSRStorage()
        self.CS_=CSRStorage()

        # # #

        self.db_=Signal(8)
        self.rs_=Signal()
        self.rd_=Signal()
        self.wr_=Signal()
        self.rst_=Signal()
        self.cs_=Signal()

        # # #

        _lcd=_LCD_i80(clk)

        self.submodules += _lcd

        self.comb += [
            _lcd.data.eq(self.DATA.storage),
            _lcd.option.eq(self.OPTION.storage),
            _lcd.addr.eq(self.ADDR.storage),
            self.BUSY.status.eq(_lcd.busy),
            _lcd.start.eq(self.START.storage),
            self.db_.eq(_lcd.db),
            self.cs_.eq(self.CS_.storage),
            self.rs_.eq(_lcd.rs),
            self.rd_.eq(_lcd.rd),
            self.wr_.eq(_lcd.wr),
            self.rst_.eq(_lcd.rst),
        ]

# First simulation
"""
DBs=Signal(8)
RSs=Signal()
RDs=Signal()
WRs=Signal()
RSTs=Signal()
clk_s=Signal()

dut=LCD_i80(32e6)

def test_bench(dut):
    yield from dut.CS_.write(1)
    yield from dut.CS_.write(0)
#----- init()
    yield from dut.ADDR.write(0x3a)
    yield from dut.DATA.write(0x55)
    yield from dut.OPTION.write(0)
    yield from dut.START.write(1)
    yield from dut.START.write(0)


    yield DBs
    yield RSs
    yield RDs
    yield WRs
    yield RSTs
    yield from dut.BUSY.read()
    for i in range(3000):
        yield

print ("init simulation 1")
run_simulation(dut, test_bench(dut), vcd_name="lcd1.vcd")
print ("end simulation 1")"""
