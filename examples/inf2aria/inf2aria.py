import sys
import eacnet
import infapi
import logging
import argparse

# handle command line arguments
parser = argparse.ArgumentParser(description='Generate an aria2c download list.')
parser.add_argument('--url', help='override infapi server url', default=infapi.base_url)
parser.add_argument('--verbose', help='use debug log level', action='store_true', default=False)
parser.add_argument('output', help='output file', nargs='?')

args = parser.parse_args()

# enable verbose logging
logging.basicConfig(format='%(levelname)s: %(message)s',
                    level=logging.DEBUG if args.verbose else logging.INFO)

# allow server url to be overridden
infapi.base_url = args.url

# wait for infapi to be ready
infapi.wait_until_ready()

# ensure eacloud is not under maintenance
services = eacnet.get_services()
state = eacnet.get_server_state()

if state['state'] != 1:
    raise Exception('Server is currently under maintenance')

# update the clock for subsequent requests
clock = eacnet.get_server_clock(update=True)

# request file information
resources = eacnet.get_resource_info()

# format to aria2 download list format
result = ''

for file in resources['file']:
    line = "{url}\n\tout={out}\n\tchecksum=sha-256={checksum}\n\n"
    result += line.format(url=file['url'], out=file['path'], checksum=file['sum'])

# write to a file if specified, else print to stdout
try:
    with open(args.output, 'w') as file:
        file.write(result)
except TypeError:
    sys.stdout.write(result)
