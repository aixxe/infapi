FROM python:3.10.5-alpine

WORKDIR /app

COPY inf2aria.py infapi.py eacnet.py requirements.txt /app/
RUN pip3 install -r requirements.txt

CMD ["python3", "inf2aria.py", "--verbose", "--url=http://infapi:8080", "output/filelist.txt"]