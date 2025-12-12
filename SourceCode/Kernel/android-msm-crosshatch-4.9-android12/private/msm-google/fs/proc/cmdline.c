#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

//decash
//static char new_command_line[2048];
////////

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	//seq_printf(m, "%s\n", "rcupdate.rcu_expedited=1 rootwait ro init=/init androidboot.bootdevice=1d84000.ufshc androidboot.baseband=sdm androidboot.keymaster=1 msm_drm.dsi_display0=dsi_s6e3ha8_cmd_display::timing0 androidboot.force_normal_boot=1 androidboot.serialno=8B9Y0VSZ3 androidboot.slot_suffix=_a androidboot.slot_retry_count=2 androidboot.slot_successful=no androidboot.hardware.platform=sdm845 androidboot.hardware=crosshatch androidboot.revision=MP1.0 androidboot.bootloader=b1c1-0.4-7617406 androidboot.hardware.sku=G013C androidboot.hardware.radio.subtype=0 androidboot.hardware.dsds=0 androidboot.secure_boot=PRODUCTION androidboot.cdt_hwid=0x05012800 androidboot.hardware.majorid=0x01 androidboot.dtb_idx=0 androidboot.dtbo_idx=13 androidboot.bootreason=reboot androidboot.hardware.ddr=4GB,Samsung,LPDDR4X androidboot.ddr_info=Samsung androidboot.ddr_size=4GB androidboot.hardware.ufs=128GB,Micron androidboot.cid=00000000 androidboot.boottime=0BLE:88,1BLL:141,1BLE:1042,2BLL:121,2BLE:498,SW:10028,KL:0,KD:72,ODT:106,AVB:245,AFTL:0 androidboot.ramdump=disabled androidboot.blockchain=disabled usbcfg.suzyq=disabled androidboot.theme=1 androidboot.hardware.pcbcfg=G650-01995-06 androidboot.hardware.devcfg=G950-00762-03 androidboot.vbmeta.device=PARTUUID=b7fc981c-7b25-4f78-848a-e5015e7b3cf5 androidboot.vbmeta.avb_version=1.1 androidboot.vbmeta.device_state=locked androidboot.vbmeta.hash_alg=sha256 androidboot.vbmeta.size=4224 androidboot.vbmeta.digest=68cd5529bb07077fee41e72d9e4bf35e89cb61be1488e08f37681a94e87f2c26 androidboot.veritymode=enforcing androidboot.verifiedbootstate=green androidboot.aftlstate=8 printk.devkmsg=on msm_rtb.filter=0x237 ehci-hcd.park=3 service_locator.enable=1 cgroup.memory=nokmem lpm_levels.sleep_disabled=1 usbcore.autosuspend=7 loop.max_part=7 androidboot.boot_devices=soc/1d84000.ufshc androidboot.super_partition=system buildvariant=user console=null");
	seq_printf(m, "%s\n", saved_command_line);
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

//decash
/*
static void remove_flag(char *cmd, const char *flag)
{
    char *start_addr, *end_addr;
		while ((start_addr = strstr(cmd, flag))) 
		{
		     end_addr = strchr(start_addr, ' ');
		     if (end_addr)
		           memmove(start_addr, end_addr + 1, strlen(end_addr));
		     else
		           *(start_addr - 1) = '\0';
		}
}	

static void remove_safetynet_flags(char *cmd)
{
   remove_flag(cmd, "androidboot.verifiedbootstate=");
   remove_flag(cmd, "androidboot.vbmeta.device_state=");
}
*/
////////

static int __init proc_cmdline_init(void)
{
	//decash
	//strcpy(new_command_line, saved_command_line);
	//remove_safetynet_flags(new_command_line);
	////////
	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
fs_initcall(proc_cmdline_init);
