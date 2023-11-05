#define REG_CYL_LO 4
#define REG_CYL_HI 5
#define REG_DEVSEL 6

#define ATADEV_UNKNOWN 0
#define ATADEV_PATA 1
#define ATADEV_PATAPI 2
#define ATADEV_SATA 3
#define ATADEV_SATAPI 4

struct DEVICE {
    unsigned short base;
    unsigned short dev_ctl;
};

void listdrivebits()
{
    struct DEVICE ctrl;
    ctrl.base = 0x1F0;
    ctrl.dev_ctl = 0x3F6;

    for (int slavebit = 0; slavebit < 2; slavebit++) {
        int devtype = detect_devtype(slavebit, &ctrl);
        if (devtype != ATADEV_UNKNOWN) {
            printf("Drive %d is available, type: %d\n", slavebit, devtype);
        }
    }   
}

void ata_soft_reset(unsigned short dev_ctl) {
    outportb(dev_ctl, 4);
    inportb(dev_ctl);
    outportb(dev_ctl, 0);
    inportb(dev_ctl);
}

int detect_devtype(int slavebit, struct DEVICE *ctrl) {
    ata_soft_reset(ctrl->dev_ctl);
    outportb(ctrl->base + REG_DEVSEL, 0xA0 | (slavebit << 4));
    inportb(ctrl->dev_ctl);

    // Wait 400ns for drive select to work
    for (int i = 0; i < 4; i++) {
        inportb(ctrl->dev_ctl);
    }

    unsigned cl = inportb(ctrl->base + REG_CYL_LO);
    unsigned ch = inportb(ctrl->base + REG_CYL_HI);

    if (cl == 0x14 && ch == 0xEB) {
        return ATADEV_PATAPI;
    } else if (cl == 0x69 && ch == 0x96) {
        return ATADEV_SATAPI;
    } else if (cl == 0 && ch == 0) {
        return ATADEV_PATA;
    } else if (cl == 0x3c && ch == 0xc3) {
        return ATADEV_SATA;
    }
    return ATADEV_UNKNOWN;
}
