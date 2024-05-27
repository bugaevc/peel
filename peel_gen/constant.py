from peel_gen.node_handler import NodeHandler

class Constant(NodeHandler):
    def __init__(self, attrs):
        self.name = attrs['name']

