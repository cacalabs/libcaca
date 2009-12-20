struct floppy_drive {
    char type[16];
    u8   status;
};

struct floppy_info {
  	int count;
    struct floppy_drive drive[2];
};

int floppy_get_info(struct floppy_info *floppy_info);
void floppy_print_info(struct floppy_info *floppy_info);

int floppy_get_status(void);