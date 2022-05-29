## inf2aria

Generates a file list usable by [aria2c](https://aria2.github.io/) for downloading beatmania IIDX INFINITAS.

### Usage

#### Docker

Ensure you've already built the infapi image, then start the containers:

```bash
docker-compose up --build
```

The output file list should be written to `output/filelist.txt` after a short time.

infapi will continue to run indefinitely after inf2aria completes, so press <kbd>Ctrl+C</kbd> to exit.

#### Standalone

Install dependencies with Pip:

```commandline
pip install -r requirements.txt
```

Generate a download list with inf2aria:

```commandline
python inf2aria.py --url=http://localhost:8080 filelist.txt
```

Download with aria2c:
```commandline
aria2c --input-file="filelist.txt" --dir="games/bm2dxi" \
  --continue=true --auto-file-renaming=false --split=5 \
  --min-split-size=2M --max-concurrent-downloads=16 \
  --max-connection-per-server=16
```