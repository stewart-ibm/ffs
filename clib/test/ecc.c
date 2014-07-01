/*
 * Copyright (c) International Business Machines Corp., 2014
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clib/assert.h>
#include <clib/type.h>
#include <clib/misc.h>

#include <clib/ecc.h>
int test_sfc_ecc()
{
        uint64_t in[] = { 0x3131313131313131, 0x3232323232323232,
                0x3333333333333333, 0x3434343434343434,
                0x3535353535353535, 0x3636363636363636,
                0x3737373737373737, 0x3838383838383838,
                0x3132333435363738, 0x3938373635343332,
                };
        printf("Function: test_sfc_ecc\n");
        printf("IN [\n");
        for (uint i=0; i<sizeof(in); i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)in)[i]);
        }
        printf("]\n");

        uint8_t out_ecc[sizeof in + sizeof in / 8];
        int rc = sfc_ecc_inject(out_ecc, sizeof out_ecc, in, sizeof in);
        printf("sz_in(%d) sz_out(%d) rc(%d)\n", sizeof in, sizeof out_ecc, rc);

        printf("OUT_ECC [\n");
        for (int i=0; i<rc; i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
        }
        printf("]\n");

        uint8_t keep_out_ecc[sizeof in + sizeof in / 8];
        memcpy(keep_out_ecc, out_ecc, sizeof keep_out_ecc);
        printf("sz_in(%d) sz_keep_out(%d) sz_out(%d)\n", sizeof in, sizeof keep_out_ecc, sizeof out_ecc);

        printf("KEEP_OUT_ECC [\n");
        for (int i=0; i<rc; i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
        }
        printf("]\n");



        uint8_t out_no_ecc[sizeof in];
        int myrc = sfc_ecc_remove(out_no_ecc, sizeof out_no_ecc,
                                          out_ecc, sizeof out_ecc);
        printf("ECC_STATUS:%d \n",myrc);
        printf("OUT_NO_ECC [\n");
        for (uint i=0; i<sizeof(out_no_ecc); i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out_no_ecc)[i]);
        }
        printf("]\n");

        for (uint x=0; x<sizeof(out_ecc);x++)
        {
                for (int c=0; c<8; c++)
                {
                        out_ecc[x] ^= 1<<c;
                        printf("OUT_CORRUPTED_ECC[%d,%d] [\n",x,c);
                        for (int i=0; i<rc; i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
                        }
                        printf("]\n");
                        myrc = sfc_ecc_remove(out_no_ecc, sizeof out_no_ecc,
                                             out_ecc, sizeof out_ecc);
                        printf("ECC_STATUS:%d \n",myrc);
                        printf("OUT_NO_ECC [\n");
                        for (uint i=0; i<sizeof(out_no_ecc); i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_no_ecc)[i]);
                        }
                        printf("]\n");

                        printf("IN_OUT_ECC [\n");
                        for (uint i=0; i<sizeof(out_ecc); i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
                        }
                        printf("]\n\n");
                        if (myrc == 0)
                        {
                                printf("For SFC Uncorrectable event\n");
                                memcpy(out_ecc, keep_out_ecc, sizeof out_ecc);

                                //return 1;
                        }
                }
        }

        return 0;
}

int test_p8_ecc()
{
        uint64_t in[] = { 0x3131313131313131, 0x3333333333333333,};
        printf("Function: test_p8_ecc\n");
        printf("IN [\n");
        for (uint i=0; i<sizeof(in); i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)in)[i]);
        }
        printf("]\n");

        uint8_t out_ecc[sizeof in + sizeof in / 8];
        int rc = p8_ecc_inject(out_ecc, sizeof out_ecc, in, sizeof in);
        printf("sz_in(%d) sz_out(%d) rc(%d)\n", sizeof in, sizeof out_ecc, rc);

        printf("OUT_ECC [\n");
        for (int i=0; i<rc; i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
        }
        printf("]\n");

        uint8_t out_no_ecc[sizeof in];
        ecc_status_t myrc = p8_ecc_remove(out_no_ecc, sizeof out_no_ecc,
                                          out_ecc, sizeof out_ecc);
        printf("ECC_STATUS:%d \n",myrc);
        printf("OUT_NO_ECC [\n");
        for (uint i=0; i<sizeof(out_no_ecc); i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out_no_ecc)[i]);
        }
        printf("]\n");

        for (uint x=0; x<sizeof(out_ecc);x++)
        {
                for (int c=0; c<8; c++)
                {
                        out_ecc[x] ^= 1<<c;
                        printf("OUT_CORRUPTED_ECC[%d,%d] [\n",x,c);
                        for (int i=0; i<rc; i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
                        }
                        printf("]\n");
                        myrc = p8_ecc_remove(out_no_ecc, sizeof out_no_ecc,
                                             out_ecc, sizeof out_ecc);
                        printf("ECC_STATUS:%d \n",myrc);
                        printf("OUT_NO_ECC [\n");
                        for (uint i=0; i<sizeof(out_no_ecc); i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_no_ecc)[i]);
                        }
                        printf("]\n");

                        printf("IN_OUT_ECC [\n");
                        for (uint i=0; i<sizeof(out_ecc); i++) {
                                if (i && i % 8 == 0) printf(" ");
                                printf("%2.2x", ((uint8_t *)out_ecc)[i]);
                        }
                        printf("]\n\n");
                        if (myrc == UNCORRECTABLE)
                        {
                                printf("Uncorrectable event\n");
                                return 1;
                        }
                }
        }

        return 0;
}

int main (void) {

        uint64_t in[] = { 0x3131313131313131, 0x3232323232323232,
                0x3333333333333333, 0x3434343434343434, };

        printf("IN [\n");
        for (uint i=0; i<sizeof(in); i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)in)[i]);
        }
        printf("]\n");

        uint8_t out[sizeof in + sizeof in / 8];

        int rc = sfc_ecc_inject(out, sizeof out, in, sizeof in);

        printf("sz_in(%d) sz_out(%d) rc(%d)\n", sizeof in, sizeof out, rc);

        printf("OUT [\n");
        for (int i=0; i<rc; i++) {
                if (i && i % 8 == 0) printf(" ");
                printf("%2.2x", ((uint8_t *)out)[i]);
        }
        printf("]\n\n");

        sfc_ecc_dump(stdout, 0, out, rc);
        test_sfc_ecc();
        test_p8_ecc();

        return 0;
}
