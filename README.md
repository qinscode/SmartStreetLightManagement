# CITS5506_SmartLightManagement

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
