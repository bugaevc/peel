import xml.sax

from peel_gen.repository import Repository, repository_map

class SaxHandler(xml.sax.handler.ContentHandler):
    def __init__(self, repo_name, repo_version):
        self.repo_name = repo_name
        self.repo_version = repo_version
        self.stack = []
        self.repo = None

    def startElement(self, name, attrs):
        if not self.stack:
            assert(name == 'repository')
            r = Repository(attrs)
            repository_map[(self.repo_name, self.repo_version)] = r
            self.stack.append(r)
            return
        top = self.stack[-1]
        if top is None:
            self.stack.append(None)
            return
        n = top.start_child_element(name, attrs)
        self.stack.append(n)

    def endElement(self, name):
        n = self.stack.pop()
        if n is None:
            return
        n.end_element()
        if not self.stack:
            assert(name == 'repository')
            assert(isinstance(n, Repository))
            self.repo = n

    def characters(self, data):
        top = self.stack[-1]
        if top is None:
            return
        top.characters(data)

