# VCFUtils
A collection of fast, efficient C-based utilities for processing and manipulating Variant Call Format (VCF) files.

## Overview

This repository hosts a suite of command-line tools designed for high-performance VCF data handling. Each tool is written in C and leverages the `htslib` library to ensure maximum speed and reliability, making them suitable for large-scale genomics pipelines. The programs are designed to be compiled into **fully standalone binaries** with no external dependencies.

---

## Tools

### 1. `vcf-af-extractor`

A high-performance program to extract Allele Frequency (AF) data from a VCF file's INFO field.

**Purpose**: This tool is a fast and memory-efficient replacement for common shell script pipelines (e.g., `zcat | grep | sed`). It reads a BGZF-compressed VCF file, preserves the header, and for each variant, it reconstructs a new, minimal line containing only the essential columns and the `AF` value. This is ideal for drastically reducing the size of large VCFs (like those from gnomAD) for downstream analyses where only allele frequency is relevant.

**Usage**:

*   `<input.vcf.bgz>`: Path to the input VCF file (must be bgzip-compressed).
*   `<output.vcf.bgz>`: Path where the smaller, processed VCF file will be saved.

---

## Installation

The tools in this repository are built from source using the provided `Makefile`. The following steps will guide you through compiling a **standalone, statically linked binary** for `vcf-af-extractor`.

### Step 1: Install Prerequisites

Ensure you have the necessary build tools and libraries installed on your system. These are required to compile `htslib` and the tools in this repository.

**On Debian/Ubuntu:**
```
sudo apt-get update
sudo apt-get install build-essential git gcc make zlib1g-dev libbz2-dev liblzma-dev libcurl4-openssl-dev libssl-dev
```
**On RedHat/CentOS/Fedora:**
```
sudo yum groupinstall "Development Tools"
sudo yum install git gcc make zlib-devel bzip2-devel xz-devel libcurl-devel openssl-devel
```

### Step 2: Download the Project and HTSlib

1.  **Clone this repository:**
    ```
    git clone https://github.com/emontele/VCF-Utils.git
    cd VCF-Utils
    ```
2.  **Download and compile HTSlib:** The tools require `htslib`. You must compile it from source to generate the static library (`libhts.a`) needed for the build.
    ```
    # Download and extract the latest htslib release
    wget https://github.com/samtools/htslib/releases/download/1.20/htslib-1.20.tar.bz2
    tar -xjf htslib-1.20.tar.bz2

    # Configure and compile htslib
    cd htslib-1.20
    ./configure
    make
    cd ..
    ```
    This process will create a `htslib-1.20` directory containing the compiled library and headers.

### Step 3: Configure the Makefile

Before compiling, you must tell the `Makefile` where to find your compiled `htslib` library.

1.  Open the `Makefile` in a text editor.
2.  Find the line `HTSLIB_DIR = /path/to/your/htslib`.
3.  Change `/path/to/your/htslib` to point to the `htslib-1.20` directory you just created. The easiest way to do this is to use a relative path.

    **Example:**
    ```
    # For a htslib folder located in the project's root directory
    HTSLIB_DIR = $(CURDIR)/htslib-1.20
    ```

### Step 4: Compile the Tool

With the `Makefile` correctly configured, simply run `make` from the root of the project directory:
```
make
```

A standalone executable named `vcf_af_extractor` will be created. You can verify that it is statically linked and does not depend on a shared `libhts.so` file by running `ldd`:
```ldd ./vcf_af_extractor```
On Linux, this should output "not a dynamic executable" or show only system libraries.


### Cleaning Up

To remove the compiled binary and any intermediate object files, run:
`make clean`
