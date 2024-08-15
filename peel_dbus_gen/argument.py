class Argument:
    def __init__(self, attrs, default_direction):
        self.name = attrs.get('name', None)
        self.direction = attrs.get('direction', default_direction)
        self.type = attrs.get('type')
