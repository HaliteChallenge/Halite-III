#!/usr/bin/env bash

set -e

source ./config.sh

gcloud compute --project "${GCLOUD_PROJECT}" \
    instance-templates create "website-instance-template-8" \
    --machine-type "${WEBSITE_MACHINE_TYPE}" \
    --network "default" \
    --metadata "^#&&#^startup-script=$(cat setup_website__startup_script.sh)" \
    --maintenance-policy "TERMINATE" \
    --service-account "${SERVICE_ACCOUNT}@${GCLOUD_PROJECT}.iam.gserviceaccount.com" \
    --image-family "${WEBSITE_IMAGE}" --image-project "${GCLOUD_PROJECT}" \
    --boot-disk-size "25" --boot-disk-type "pd-ssd" \
    --tags "website" \
    --preemptible \
    --scopes "https://www.googleapis.com/auth/devstorage.read_only","https://www.googleapis.com/auth/logging.write","https://www.googleapis.com/auth/monitoring.write","https://www.googleapis.com/auth/servicecontrol","https://www.googleapis.com/auth/service.management.readonly","https://www.googleapis.com/auth/trace.append" \
    --tags "website-backend"

gcloud compute --project "${GCLOUD_PROJECT}" \
    instance-groups managed create "website-instances-prodxi" \
    --zone "${GCLOUD_ZONE}" \
    --base-instance-name "website-instances" \
    --template "website-instance-template-8" --size "1"

gcloud compute --project "${GCLOUD_PROJECT}" \
    instance-groups managed set-autoscaling "website-instances-prodxi" \
    --zone "${GCLOUD_ZONE}" \
    --cool-down-period "60" \
    --max-num-replicas "10" --min-num-replicas "3" \
    --target-cpu-utilization "0.8"