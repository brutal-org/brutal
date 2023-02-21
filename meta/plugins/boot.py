import os



from osdk.context import loadAllComponents
from osdk import utils, shell, builder
from osdk.cmds import Cmd, append
from osdk.args import Args


def kvmAvailable() -> bool:
    if os.path.exists("/dev/kvm") and os.access("/dev/kvm", os.R_OK):
        return True
    return False



def buildPkgs(binDir: str, debug: bool) -> list[str]:
    pkgs = [p.id for p in loadAllComponents() if "src/srvs" in p.dirname()]

    for pkg in pkgs:
        elf = builder.build(pkg, "brutal-x86_64:debug")
        shell.cp(elf, f"{binDir}/{os.path.basename(elf)[:-4]}")



    return pkgs


def limineGenConfig(bootDir: str, pkgs: list[str]) -> None:
    with open(f"{bootDir}/limine.cfg", "w") as cfg:
        cfg.write(
            "TIMEOUT=0\n:Munix\nPROTOCOL=limine\nKERNEL_PATH=boot:///boot/kernel.elf\n"
        )

        for pkg in pkgs:
            cfg.write(f"MODULE_PATH=boot:///bin/{pkg}")


def copySysroot(sysrootDir: str, imageDir: str) -> None:
    for root, _, files in os.walk(sysrootDir):
        for f in files:
            shell.mkdir(f"{imageDir}/{os.path.relpath(root, sysrootDir)}")
            shell.cp(os.path.join(root, f), f"{imageDir}/{os.path.relpath(root, sysrootDir)}/{f}")

def bootCmd(args: Args) -> None:
    debug = "debug" in args.opts
    imageDir = shell.mkdir(".osdk/images/efi-x86_64")
    efiBootDir = shell.mkdir(f"{imageDir}/EFI/BOOT")
    binDir = shell.mkdir(f"{imageDir}/bin")
    bootDir = shell.mkdir(f"{imageDir}/boot")

    ovmf = shell.wget("https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd")

    brutal = builder.build("core", "kernel-x86_64:debug")
    loader = builder.build("loader-brutal", "loader-x86_64:debug")


    shell.cp(brutal, f"{bootDir}/kernel.elf")
    shell.cp(loader, f"{efiBootDir}/BOOTX64.EFI")


    copySysroot("./sysroot", f"{imageDir}")
    buildPkgs(f"{imageDir}/pkgs", debug)




    qemuCmd: list[str] = [
        "qemu-system-x86_64",
        "-machine",
        "q35",
        "-no-reboot",
        "-no-shutdown",
        # "-d", "guest_errors,cpu_reset,int",
        # "-S", "-s",
        "-serial",
        "mon:stdio",
        "-bios",
        ovmf,
        "-m",
        "4G",
        "-smp",
        "4",
        "-drive",
        f"file=fat:rw:{imageDir},media=disk,format=raw",
    ]

    if kvmAvailable():
        qemuCmd += ["-enable-kvm"]
    else:
        print("KVM not available, using QEMU-TCG")

    shell.exec(*qemuCmd)

def protocolBuild(args: Args) -> None:
    elf = builder.build("idgen", "host-x86_64:debug")
        
    pkgs = loadAllComponents()
    for pkg in pkgs:
        if "" in pkg.dirname():
            shell.exec(elf, pkg.id, f"{pkg.dirname()}/protocol.h")

append(Cmd("p", "protocols", "Build protocols headers", buildPkgs))
append(Cmd("s", "start", "Boot the system", bootCmd))