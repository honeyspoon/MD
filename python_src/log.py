import logging

class CustomFormatter(logging.Formatter):
    grey = "\x1b[38;20m"
    green = "\033[32m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"

    COLORS = {
        logging.DEBUG: grey,
        logging.INFO: green,
        logging.WARNING: yellow,
        logging.ERROR: red,
        logging.CRITICAL: bold_red
    }

    def format(self, record):
        reset = "\x1b[0m"
        color = self.COLORS[record.levelno]
        log_fmt = f"%(asctime)s - [{color}%(levelname)s{reset}] - %(filename)s:%(lineno)d - %(message)s"
        formatter = logging.Formatter(log_fmt, datefmt="%Y-%m-%d %H:%M:%S")
        return formatter.format(record)

logger = logging.getLogger("My_app")
logger.setLevel(logging.DEBUG)

ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)

ch.setFormatter(CustomFormatter())

logger.addHandler(ch)

def info(*args):
    logger.info(*args)

def warn(*args):
    logger.warn(*args)

def error(*args):
    logger.error(*args)


