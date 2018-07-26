from .. import config

if __name__ == "__main__":
    for (region, instance_name, port) in config.DATABASES:
        print("{} {}:{}:{}".format(port,
                                   config.DATABASE_PROJECT_ID,
                                   region,
                                   instance_name))
