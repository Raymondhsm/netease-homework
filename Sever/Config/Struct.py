class Vector3:
    def __init__(self, dict = {'x':0.0, 'y':0.0, 'z':0.0}):
        self.x = dict['x']
        self.y = dict['y']
        self.z = dict['z']

    def toDict(self):
        return {
            'x': self.x,
            'y': self.y,
            'z': self.z
        }