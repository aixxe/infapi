import json
import time
import base64
import requests
import xmltodict
from urllib.parse import urljoin

# url to the infapi server
base_url = 'http://localhost:8080'


# wait until the server is ready to start handling requests
def wait_until_ready():
    while True:
        try:
            if requests.get(url=base_url).status_code == 200:
                break
        except requests.exceptions.ConnectionError:
            pass

        time.sleep(1)


# encode and sign a request for submission
def encode_request(method, params=None):
    body = {
        'p2d': {
            'method': {
                '@__type': 'str',
                '#text': method
            },
        }
    }

    if params is not None:
        body['p2d']['params'] = params

    # convert body to XML, then wrap it in JSON
    body = json.dumps({'body': xmltodict.unparse(body)})

    # submit to infapi for encoding and return the response
    response = requests.post(url=urljoin(base_url, 'request'), data=body)
    return response.json()


# decode binary response to dictionary
def decode_response(content):
    body = json.dumps({'body': base64.b64encode(content).decode('utf-8')})
    response = requests.post(url=urljoin(base_url, 'response'), data=body)
    return xmltodict.parse(response.json().get('body'))


# update the eacnet clock (ensures generated signatures are valid)
def set_clock(value):
    requests.post(url=urljoin(base_url, 'clock'), data=json.dumps({'value': value}))
