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

