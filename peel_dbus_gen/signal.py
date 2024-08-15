class Signal:
    def __init__(self, attrs):
        self.name = attrs.get('name')
        self.arguments = []

    def generate_header(self):
        return '/* TODO: {} */'.format(self.name)

    def generate_vfunc_ptr(self):
        return '/* TODO: {} */'.format(self.name)
