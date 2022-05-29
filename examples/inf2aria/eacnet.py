import time
import infapi
import logging
import requests


# minimal effort for looking like a real client
GLOBAL_HEADERS = {
    'Content-Type': 'application/x-www-form-urlencoded',
    'User-Agent': 'e-AMUSEMENT CLOUD AGENT',
}

# sent as part of every request. i don't think this has ever changed
PROTOCOL_VERSION = "P2D:2015091800"

# storage for service routes -- initially start with only the entrypoint and populate later by calling get_services()
# the getServices URL here can be extracted from any game binary, e.g. `strings bm2dx.exe | grep GetServices`
_services = {
    'getServices': 'https://p.eagate.573.jp/game/eac2dx/infinitas/APIEX/servletex2/preProcess/GetServices',
}


# generate post form data for a request
def _get_body(request, signature):
    body = 'client_version=&protocol_version={protocol_version}&request={request}&retry_count=0&signature={signature}'
    return body.format(protocol_version=PROTOCOL_VERSION, request=request, signature=signature)


# convenience method since a few methods use the same parameters
def _get_dummy():
    return {
        '@__type': 'u8',
        '#text': 0
    }


# encode, submit and decode a request
def _perform(method, params=None):
    url = _services[method]

    encode_start = time.time()
    request = infapi.encode_request(method, params)
    logging.info('encoded request \'%s\' in %.3f ms', method, (time.time() - encode_start) * 1000.0)

    submit_start = time.time()
    body = _get_body(request['request'], request['signature'])
    response = requests.post(url, data=body, headers=GLOBAL_HEADERS)
    logging.info('submitted request \'%s\' in %.3f ms', method, (time.time() - submit_start) * 1000.0)

    decode_start = time.time()
    result = infapi.decode_response(response.content)
    logging.info('decoded response \'%s\' in %.3f ms', method, (time.time() - decode_start) * 1000.0)

    return result


# populate _services with routes and return a copy
def get_services():
    result = _perform('getServices', _get_dummy())

    _services.clear()

    for service in result['p2d']['result']['service']:
        name = service['service_name']['#text']
        url = service['url'].get('#text')

        _services[name] = url

    return _services.copy()


# used to determine whether the server is under maintenance
def get_server_state():
    result = _perform('getServerState')

    return {
        'state': int(result['p2d']['result']['server_state']['#text']),
        'mainte_start_clock': int(result['p2d']['result']['mainte_start_clock']['#text']),
        'mainte_end_clock': int(result['p2d']['result']['mainte_end_clock']['#text']),
    }


# retrieve the server clock (and optionally use it for subsequent requests)
def get_server_clock(update=False):
    result = int(_perform('getServerClock')['p2d']['result']['server_clock']['#text'])

    if update is True:
        infapi.set_clock(result)

    return result


# retrieve various data for base game files
def get_resource_info():
    result = _perform('getResourceInfo', {
        'full': {
            '@__type': 'bool',
            '#text': '1'
        }
    })
    resources = {
        'file': [],
        'hash': result['p2d']['result']['hash']['#text'],
        'version': result['p2d']['result']['resource_info']['ri_ver']['#text'],
    }

    for resource in result['p2d']['result']['resource_info']['file']:
        resources['file'].append({
            'path': resource['path']['#text'],
            'version': resource['version']['#text'],
            'size': int(resource['size']['#text']),
            'sum': resource['sum']['#text'],
            'url': resource['url']['#text'],
        })

    return resources
