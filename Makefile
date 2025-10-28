KERNEL_DIR = kernel
OBJS = $(KERNEL_DIR)/entry.o $(KERNEL_DIR)/main.o $(KERNEL_DIR)/console.o $(KERNEL_DIR)/string.o $(KERNEL_DIR)/fs.o $(KERNEL_DIR)/shell.o

CROSS = riscv64-linux-gnu-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
OBJDUMP = $(CROSS)objdump

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles \
         -march=rv64imac -mabi=lp64 -mcmodel=medany -I$(KERNEL_DIR)
LDFLAGS = -T $(KERNEL_DIR)/kernel.ld -z max-page-size=4096

# Build final ELF
$(KERNEL_DIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Assemble .s -> .o
$(KERNEL_DIR)/entry.o: $(KERNEL_DIR)/entry.s
	$(CC) -c -o $@ $< -march=rv64imac -mabi=lp64

# Compile C sources
$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Run in QEMU
run: $(KERNEL_DIR)/kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $(KERNEL_DIR)/kernel.elf -nographic

clean:
	rm -f $(KERNEL_DIR)/*.o $(KERNEL_DIR)/*.elf
