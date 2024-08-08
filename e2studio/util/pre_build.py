#!/usr/bin/python3
#
# Pre-build script to generate certificates and webserver FS blob.
#
#  Created on: Aug 8, 2024
#      Author: evoirin

import os
import platform

def main():
    #   Create webserver FS blob
    #   TODO: Reverse engineer this and implement for linux maybe?

    if platform.system() == "Windows":
        os.system(f'EmbedFS.exe -l -g -x -i "..\src\webserver\website\*" -o "..\src\webserver" -f "fswebsite.bin"')
    else:
        print(f'Generating Webserver FS blob not implemented on operating system "{platform.system()}", skipping...')

    #   Generate certificate header file

    with open('../cert/iotc_demo_dev_cert.h', 'w') as of:
        of.write('/* AUTO-GENERATED, DO NOT EDIT */\n')

        if os.path.isfile('../cert/device_cert.pem') and os.path.isfile('../cert/device_key.pem'):
            print("Using client certificate and key PEM files.")

            with open('../cert/device_cert.pem', 'r') as certf, open('../cert/device_key.pem', 'r') as keyf:
                of.write('#define __DEVICE_CERT__ \\\n')
                for line in certf:
                    line = line.replace('\r', '')
                    line = line.replace('\n', '')
                    of.write(f'"{line}" \\\n')

                of.write('\n')

                of.write('#define __DEVICE_KEY__ \\\n')
                for line in keyf:
                    line = line.replace('\r', '')
                    line = line.replace('\n', '')
                    of.write(f'"{line}" \\\n')
                    
                of.write('\n')

        else:
            # Generate empty header file if PEMs are not found
            print("Client certificate and key not found; not using them...")
            of.write('/* Not using device cert / key. */\n')


if __name__ == "__main__":
    main()

