class Property:
    def __init__(self, attrs):
        self.name = attrs.get('name')
        self.type = attrs.get('type')
        self.access = attrs.get('access')

    def generate_header(self):
        return '/* TODO: {} */'.format(self.name)
