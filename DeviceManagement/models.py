
from django.db import models

class LoraMessage(models.Model):
    device_id = models.CharField(max_length=100)
    event = models.TextField()
    update_time = models.DateTimeField()
    brightness = models.PositiveIntegerField()
    switch = models.PositiveIntegerField()
    maintenance = models.PositiveIntegerField()
    location = models.CharField(max_length=100)

    def to_dict(self):
        return {
            'device_id': self.device_id,
            'event': self.event,
            'update_time': self.update_time.strftime('%Y-%m-%d %H:%M:%S'),
            'brightness': self.brightness,
            'switch': self.switch,
            'maintenance': self.maintenance,
            'location': self.location
        }