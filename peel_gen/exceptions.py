class UnsupportedForNowException(Exception):
    def __init__(self, reason):
        self.reason = reason
