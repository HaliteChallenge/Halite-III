# Placed here to avoid circular imports
def read_input():
    """
    Reads input from stdin, shutting down logging and exiting if an EOFError occurs
    :return: input read
    """
    try:
        return input()
    except EOFError as eof:
        logging.shutdown()
        raise SystemExit(eof)
