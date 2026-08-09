#define __FILES_C_
#include "files.h"

//fatfs
FATFS fs;
__attribute((aligned(8))) static uint32_t workbuf[4096];
MKFS_PARM fs_para = {
    .fmt = FM_FAT32,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 1,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};
static uint8_t is_fatfs_init_ok = 0;

int filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);
    // ret = FR_NO_FILESYSTEM;

    if (ret == FR_NO_FILESYSTEM) {
        printf("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            printf("fail to make filesystem %d\r\n", ret);
            // _CALL_ERROR();
            return ret;
        }

        if (ret == FR_OK) {
            printf("done with formatting.\r\n");
            printf("first start to unmount.\r\n");
            ret = f_mount(NULL, "/sd", 1);
            printf("then start to remount.\r\n");
            if (ret == FR_OK) {
                ret = f_mount(&fs, "/sd", 1);
            }
        }
    } else if (ret != FR_OK) {
        printf("fail to mount filesystem,error= %d\r\n", ret);
        printf("SD card might fail to initialise.\r\n");
        // _CALL_ERROR();
        return ret;
    } else {
        printf("Succeed to mount filesystem\r\n");
    }

    if (ret == FR_OK) {
        printf("FileSystem cluster size:%d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);
    }

    is_fatfs_init_ok = 1;
    return 0;
}

int take_photo(uint8_t *Buf, uint32_t len)
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;
    // uint32_t time_node;
    static uint32_t create_file_count = 0;
#define PHOTO_NAME_LEN 30
    char file_name[PHOTO_NAME_LEN] = { 0 };
    uint8_t *pData;
    uint32_t i = 0;
    UINT write_size;

    memset(file_name, 0, PHOTO_NAME_LEN);
    snprintf(file_name, PHOTO_NAME_LEN, "/sd/%lu.jpg", (unsigned long)create_file_count);
    create_file_count++;
    ret = f_open(&fnew, file_name, FA_CREATE_ALWAYS | FA_WRITE);
    // ret = f_open(&fnew, "/sd/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (ret == FR_OK) {
        // time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        // ret = f_write(&fnew, Buf, len, &fnum);
        pData = Buf;
        while (i < len) {
            write_size = (len - i) < (10 * 1024) ? (len - i) : (10 * 1024);
            ret = f_write(&fnew, pData, write_size, &fnum);
            if ((ret != FR_OK) || (fnum != write_size)) {
                printf("Fail to write files(%d), requested:%u, written:%u\n", ret, write_size, fnum);
                f_close(&fnew);
                return (ret != FR_OK) ? ret : FR_DISK_ERR;
            }
            i += fnum;
            pData += fnum;
        }

        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        // time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            printf("Write Succeed! photo cnt:%lu\r\n", (unsigned long)(create_file_count - 1));
            printf("Write data size:%lu Byte\r\n", (unsigned long)len);
            // printf("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            printf("Fail to write files(%d)\n", ret);
            return ret;
        }
    } else {
        printf("Fail to open or create files: %d.\r\n", ret);
        return ret;
    }

    return FR_OK;
}
