from migen import *

from migen.genlib.io import CRG

from litex.build.generic_platform import *
from litex.build.xilinx import XilinxPlatform

import litex.soc.integration.soc_core as SC
from litex.soc.integration.builder import *

from litex.soc.cores.spi import SPIMaster
from lcd_core import *
from btn_itrupt import btnintrupt



_io = [

    ("clk32", 0, Pins("P126"), IOStandard("LVCMOS33")),

    ("cpu_reset", 0, Pins("P87"), IOStandard("LVCMOS33")),


    ("serial", 0,
        Subsignal("tx", Pins("P105")),
        Subsignal("rx", Pins("P101")),
        IOStandard("LVCMOS33"),
    ),

    ("user_btn", 0, Pins("P74"), IOStandard("LVCMOS33")),
    ("user_btn", 1, Pins("P78"), IOStandard("LVCMOS33")),
    ("user_btn", 2, Pins("P80"), IOStandard("LVCMOS33")),
    ("user_btn", 3, Pins("P62"), IOStandard("LVCMOS33")),
    ("user_btn", 4, Pins("P65"), IOStandard("LVCMOS33")),

    ("rst",0, Pins("P35"),IOStandard("LVCMOS33")),
    ("cs",0, Pins("P39"),IOStandard("LVCMOS33")),
    ("rs",0, Pins("P79"),IOStandard("LVCMOS33")),
    ("wr",0, Pins("P81"),IOStandard("LVCMOS33")),
    ("rd",0, Pins("P84"),IOStandard("LVCMOS33")),
    ("db",0, Pins("P32 P34 P26 P29 P30 P33 P24 P27"),IOStandard("LVCMOS33")),
    ("SD", 0,
        Subsignal("cs_n", Pins("P92")),
        Subsignal("clk", Pins("P94")),
        Subsignal("mosi", Pins("P93")),
        Subsignal("miso", Pins("P95")),
        IOStandard("LVCMOS33")
    ),
    ("uCD", 0, Pins("P97"), IOStandard("LVCMOS33"))



]

class Platform(XilinxPlatform):
    default_clk_name = "clk32"
    default_clk_period = 31.25

    def __init__(self):
        XilinxPlatform.__init__(self, "xc6slx9-TQG144-2", _io, toolchain="ise")

    def do_finalize(self, fragment):
        XilinxPlatform.do_finalize(self, fragment)


def csr_map_update(csr_map, csr_peripherals):
    csr_map.update(dict((n, v)
        for v, n in enumerate(csr_peripherals, start=max(csr_map.values()) + 1)))


platform = Platform()

# create our soc (fpga description)
class BaseSoC(SC.SoCCore):
    # Peripherals CSR declaration
    csr_peripherals = [
        "lcd_test",
        "buttons",
        "spi_sd",
    ]
    csr_map_update(SC.SoCCore.csr_map, csr_peripherals)


    interrupt_map = {
        "buttons" : 4,
    }

    interrupt_map.update(interrupt_map)

    def __init__(self, platform):
        sys_clk_freq = int(32e6)

        SC.SoCCore.__init__(self, platform,
            cpu_type="lm32",
            clk_freq=32e6,
            csr_data_width=32,
            ident="GameSnake", ident_version=True,
            integrated_rom_size=0x8000,
            integrated_main_ram_size=16*1024)

        # Clock Reset Generation
        self.submodules.crg = CRG(platform.request("clk32"), ~platform.request("cpu_reset"))

        #Button_interrupt
        button_in = Cat(*[platform.request("user_btn", i) for i in range(5)])
        self.submodules.buttons = btnintrupt(button_in)

        #lcd parallel
        self.submodules.lcd_test = LCD_i80(sys_clk_freq)
        self.comb += [
            platform.request("db").eq(self.lcd_test.db_),
            platform.request("cs").eq(self.lcd_test.cs_),
            platform.request("rs").eq(self.lcd_test.rs_),
            platform.request("rd").eq(self.lcd_test.rd_),
            platform.request("wr").eq(self.lcd_test.wr_),
            platform.request("rst").eq(self.lcd_test.rst_),
           ]

        # SD_spi
        self.submodules.spi_sd = SPIMaster(platform.request("SD"))
        self.comb+=platform.request("uCD").eq(0x1)
      
soc = BaseSoC(platform)

#
# build
#

builder = Builder(soc, output_dir="build", csr_csv="test/csr.csv")
builder.build()
