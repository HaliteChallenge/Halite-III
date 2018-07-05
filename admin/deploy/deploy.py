#!/usr/bin/env python3
import json
import sys

from google.cloud import storage

with open("../../../deploy.json") as cfgfile:
    config = json.load(cfgfile)

client = storage.Client()
bucket = client.get_bucket(config["bucket"])
current_blob = bucket.get_blob("Halite.tgz")
new_name = current_blob.time_created.strftime("Halite-%Y-%m-%d-%H-%M.tgz")

if bucket.get_blob(new_name):
    print("Tarball {} already exists, aborting!".format(new_name))
    sys.exit(1)

bucket.rename_blob(current_blob, new_name)

new_blob = bucket.blob("Halite.tgz")
with open("../../../Halite.tgz", "rb") as blobfile:
    new_blob.upload_from_file(blobfile)
