# CITS5506_SmartLightManagement
A very simple device management system for CITS5506 project.<br/>
<img src="https://github.com/realfudong/CITS5506_SmartLightManagement/blob/main/pics/IMG_9117.JPG" width=200px><img src="https://github.com/realfudong/CITS5506_SmartLightManagement/blob/main/pics/IMG_9116.JPG" width=200px><img src="https://github.com/realfudong/CITS5506_SmartLightManagement/blob/main/pics/IMG_9115.JPG" width=200px>


## Project Requirement
- Python 3.9.13
- Django 4.2.6
- Database connector for Python (Like MySQL, SQLite...)

## Note
- Get your database ready and change `SmartLightManagement/Settings.py` and `manage.py` before use. In my case, Sensitive data will be store in `config.json`. 
- Make sure run `python manage.py makemigrations` and `python manage.py migrate`.

## Funtion
- A dashboard for street light management
- A detail page for each device
- History log
- Device status
- Trend of brightness over time

## Database Schema
- device_id
- brightness 0-100%
- update_time
- event 
- maintenance 1:OK 0:ERROR
- switch 1:ON 0:OFF
- location	
