"""
Grab worker configuration from GCloud instance attributes.
"""
import json
import requests
import secrets


MANAGER_URL_METADATA_URL = "http://metadata.google.internal/computeMetadata/v1/instance/attributes/halite-manager-url"
SECRET_FOLDER_METADATA_URL = "http://metadata.google.internal/computeMetadata/v1/instance/attributes/halite-secret-folder"
GPU_CAPABILITY_METADATA_URL = "http://metadata.google.internal/computeMetadata/v1/instance/attributes/halite-gpu"
MAX_UPLOAD_SIZE_METADATA_URL = "http://metadata.google.internal/computeMetadata/v1/instance/attributes/halite-max-upload-size"

MANAGER_URL = requests.get(MANAGER_URL_METADATA_URL, headers={
    "Metadata-Flavor": "Google"
}).text
SECRET_FOLDER = requests.get(SECRET_FOLDER_METADATA_URL, headers={
    "Metadata-Flavor": "Google"
}).text
HAS_GPU = requests.get(GPU_CAPABILITY_METADATA_URL, headers={
    "Metadata-Flavor": "Google"
}).text == "true"
MAX_UPLOAD_SIZE = requests.get(GPU_CAPABILITY_METADATA_URL, headers={
    "Metadata-Flavor": "Google"
}).text

try:
    MAX_UPLOAD_SIZE = int(MAX_UPLOAD_SIZE)
except:
    MAX_UPLOAD_SIZE = None

try:
    # Use pre-existing id if available
    with open("config.json", "r") as configfile:
        oldconfig = json.load(configfile)
        RANDOM_ID = oldconfig["RANDOM_ID"]
except Exception:
    # Couldn't load one so generate it instead
    alphabet = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPRSTUVWXYZ123456789"
    RANDOM_ID = ''.join(secrets.choice(alphabet) for n in range(11))

with open("config.json", "w") as configfile:
    json.dump({
        "MANAGER_URL": MANAGER_URL,
        "SECRET_FOLDER": SECRET_FOLDER,
        "CAPABILITIES": ["gpu"] if HAS_GPU else [],
        "MAX_BOT_UPLOAD_SIZE": MAX_UPLOAD_SIZE,
        "RANDOM_ID": RANDOM_ID,
    }, configfile)
