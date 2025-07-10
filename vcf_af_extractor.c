/**
 * @file vcf_af_extractor.c
 * @brief A fast VCF processor to extract Allele Frequency (AF) from the INFO field.
 *
 * It reads a
 * BGZF-compressed VCF file, keeps the header, and for each data line, it
 * reconstructs the line keeping only essential columns and the AF value from
 * the INFO field.
 *
 * Dependencies:
 * This program requires the htslib library to be installed.
 * On Debian/Ubuntu: sudo apt-get install libhts-dev
 * On RedHat/CentOS: sudo yum install htslib-devel
 * On MacOS (Homebrew): brew install htslib
 *
 * Compilation example:
 * gcc -Wall -O3 -Ihtslib/include -o vcf_af_extractor vcf_af_extractor.c htslib/lib/libhts.a -lpthread -lz -lbz2 -llzma -lcurl -lcrypto -ldeflate -lm
 * Or use the provided Makefile: make
 *
 * Usage:
 * ./vcf_af_extractor <input.vcf.bgz> <output.vcf.bgz>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htslib/bgzf.h"
#include "htslib/kstring.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.vcf.bgz> <output.vcf.bgz>\n", argv[0]);
        return 1;
    }

    const char *input_vcf_path = argv[1];
    const char *output_vcf_path = argv[2];

    BGZF *in_fp = bgzf_open(input_vcf_path, "r");
    if (in_fp == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_vcf_path);
        return 1;
    }

    BGZF *out_fp = bgzf_open(output_vcf_path, "w");
    if (out_fp == NULL) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_vcf_path);
        bgzf_close(in_fp);
        return 1;
    }

    // kstring_t is a dynamic string from htslib, useful for reading lines
    kstring_t line = {0, 0, NULL};
    
    // --- Main Processing Loop ---
    while (bgzf_getline(in_fp, '\n', &line) > 0) {
        // If the line is a header line (starts with '#'), write it directly to the output
        if (line.s[0] == '#') {
            if (bgzf_write(out_fp, line.s, line.l) < 0 || bgzf_write(out_fp, "\n", 1) < 0) {
                 fprintf(stderr, "Error: Failed to write header to output file.\n");
                 break;
            }
            continue;
        }

        // --- Data Line Parsing ---
        char *line_copy = strdup(line.s);
        if (line_copy == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for line copy.\n");
            break;
        }

        char *p = line_copy;
        char *fields[8];
        int col_idx = 0;

        // Split the line into columns by tab characters.
        while (col_idx < 8 && (fields[col_idx] = strsep(&p, "\t")) != NULL) {
            col_idx++;
        }

        // Check if we have at least 8 columns. The 8th column is INFO (index 7).
        if (col_idx < 8) {
            free(line_copy);
            continue; // Malformed line, skip it.
        }
        
        // --- AF Value Extraction ---
        char *info_field = fields[7];
        char *af_tag_ptr = strstr(info_field, "AF=");

        
        // If "AF=" is not found drop the line.
        if (af_tag_ptr == NULL) {
            free(line_copy);
            continue;
        }

        // Pointer to the start of the AF *value* (3 chars after "AF=")
        char *af_val_ptr = af_tag_ptr + 3;
        
        // Find the end of the AF value, which is either a semicolon or the end of the string.
        char *af_val_end_ptr = strchr(af_val_ptr, ';');
        
        kstring_t new_info = {0, 0, NULL};
        kputs("AF=", &new_info);

        if (af_val_end_ptr != NULL) {
            // AF is followed by other info fields; copy only the AF value.
            kputsn(af_val_ptr, af_val_end_ptr - af_val_ptr, &new_info);
        } else {
            // AF is the last or only info field.
            kputs(af_val_ptr, &new_info);
        }

        // --- Construct and Write New Line ---
        // Reconstruct the line
        // CHROM, POS, ID(.), REF, ALT, QUAL(.), FILTER, INFO(AF=...)
        kstring_t output_line = {0, 0, NULL};
        ksprintf(&output_line, "%s\t%s\t.\t%s\t%s\t.\t%s\t%s\n",
                 fields[0], fields[1], fields[3], fields[4], fields[6], new_info.s);

        if (bgzf_write(out_fp, output_line.s, output_line.l) < 0) {
            fprintf(stderr, "Error: Failed to write data to output file.\n");
            free(line_copy);
            free(new_info.s);
            free(output_line.s);
            break;
        }

        // --- Cleanup for this line ---
        free(line_copy);
        free(new_info.s);
        free(output_line.s);
    }

    // --- Final Cleanup ---
    fprintf(stdout, "Processed %s -> %s\n", input_vcf_path, output_vcf_path);
    free(line.s);
    bgzf_close(in_fp);
    bgzf_close(out_fp);

    return 0;
}
