@echo off

echo /clock: Updating clock value...
curl -d "{\"value\":1642772720510}" http://localhost:8080/clock
echo.
echo.

echo /request: Encoding 'getServices' request...
curl -d @requests/getServices.json http://localhost:8080/request
echo.
echo.

echo /request: Encoding 'getServerState' request...
curl -d @requests/getServerState.json http://localhost:8080/request
echo.
echo.

echo /request: Encoding 'getServerClock' request...
curl -d @requests/getServerClock.json http://localhost:8080/request
echo.
echo.

echo /request: Encoding 'checkSendLogAvailable' request...
curl -d @requests/checkSendLogAvailable.json http://localhost:8080/request
echo.
echo.

echo /request: Encoding 'getResourceInfo' request...
curl -d @requests/getResourceInfo.json http://localhost:8080/request
echo.
echo.

echo /response: Decoding 'getServerClock' response...
curl -d @responses/getServerClock.json http://localhost:8080/response
echo.
echo.

pause