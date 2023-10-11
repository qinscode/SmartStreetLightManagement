from azure.iot.hub import IoTHubRegistryManager
from datetime import datetime
import pytz
import mysql.connector
import time
import os
import json

def convert_utc_to_local(time_string, local_timezone):
    # Split UTC time string to remove fractional seconds
    utc_time = time_string.split(".")[0]

    # Parse UTC time string to datetime
    formatted_time = datetime.strptime(utc_time, '%Y-%m-%dT%H:%M:%S')

    # Convert UTC time to local time
    date_object_local = formatted_time.replace(tzinfo=pytz.utc).astimezone(local_timezone)

    return date_object_local


def list_device_twin_properties(iot_hub_connection_string):
    try:
        # Initialize IoTHubRegistryManager
        registry_manager = IoTHubRegistryManager(iot_hub_connection_string)

        # Get a list of devices
        devices = registry_manager.get_devices()
        device = devices[0]

        # Get the device's twin
        twin = registry_manager.get_twin(device.device_id)

        # Extract relevant properties
        time_string = twin.properties.reported['lastSeenAt']
        text = twin.properties.reported['decodedPayload']['myText']
        device_id = twin.device_id

        cleaned_text = ''.join(text.split())
        try:
            raw_data = json.loads(cleaned_text)
        except json.JSONDecodeError as e:
            print(f"The type of Event is not json: {e}")
            return None
        except Exception as e:
            print(f"An error occurred: {e}")
            return None


        brightness = raw_data["brightness"]
        switch = raw_data["switch"]
        maintenance = 0
        event = raw_data["event"]
        if "maintenance" in raw_data:
            maintenance = raw_data["maintenance"]

        # Define Perth timezone
        perth_timezone = pytz.timezone('Australia/Perth')

        # Convert UTC time to local time
        update_time_local = convert_utc_to_local(time_string, perth_timezone).strftime("%Y-%m-%d %H:%M:%S")

        result = {
            "device_id": device_id,
            "event": event,
            "update_time": datetime.strptime(update_time_local, '%Y-%m-%d %H:%M:%S'),
            "brightness": brightness,
            "switch": switch,
            "maintenance": maintenance
        }

        return result

    except Exception as e:
        # Handle exceptions as needed
        print(f"An error occurred: {str(e)}")
        return None  # Return None to indicate an error


def insert_data_into_database(data_dict):

    connection = mysql.connector.connect(**db_config)
    cursor = connection.cursor()
    try:
        if data_dict.get("device_id") == "eui-70b3d57ed0058753":
            data_dict['location'] = '67 William St'
        elif data_dict.get("device_id") == "eui-70b3d57ed0061948":
            data_dict['location'] = '128 Newcastle St'
        elif data_dict.get("device_id") == "eui-70b3d57ed0046532":
            data_dict['location'] = '12 Hay St'

        # Define the SQL query to insert data into a table
        insert_query = "INSERT INTO `iot_app_loramessage` (device_id, event, update_time, brightness, switch, maintenance,location) VALUES (%(device_id)s, %(event)s, %(update_time)s, %(brightness)s, %(switch)s, %(maintenance)s, %(location)s)"

        cursor.execute(insert_query, data_dict)

        connection.commit()
        connection.close()
        print("Data inserted into Azure MySQL database successfully.")

    except mysql.connector.IntegrityError as ex:
        if "1062 (23000)" in str(ex):
            print("Error: Duplicate entry for update_time. This field should be unique.")
        else:
            print(f"Error inserting data into the database: {ex}")

    except Exception as ex:
        print(f"Error inserting data into the database: {ex}")

def get_latest_record():

    connection = mysql.connector.connect(**db_config)
    cursor = connection.cursor()
    try:
        # Define the SQL query to retrieve the latest record
        select_query = "SELECT * FROM `iot_app_loramessage` ORDER BY update_time DESC LIMIT 1"

        # Execute the SQL query
        cursor.execute(select_query)

        # Fetch the result
        latest_record = cursor.fetchone()

        if latest_record is not None:
            column_names = [description[0] for description in cursor.description]
            latest_record_dict = {column_names[i]: latest_record[i] for i in range(len(column_names))}
            connection.close()

            return latest_record_dict
        else:
            connection.close()

            return None

    except Exception as ex:
        print(f"Error while retrieving the latest record: {ex}")
        connection.close()
        return None

if __name__ == "__main__":
    db_config = {
        "host": "iot-server.mysql.database.azure.com",
        "user": "iot",
        "password": "baton-ague-WINCH",
        "database": "iot-database",
    }
    CONNECTION_STRING = "HostName=iothub-utoipfc5sv5ho.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=25ebmPI+9QJ8CzcXKhB6tVUqv9U9Qc8emAIoTPF+9CI="

    latest_record_dict = get_latest_record()
    last_update_time = latest_record_dict.get('update_time')

    while True:
        data_dict = list_device_twin_properties(CONNECTION_STRING)
        if data_dict is None:
            continue
        if data_dict.get('update_time') == last_update_time:
            continue

        insert_data_into_database(data_dict)

        last_update_time = data_dict.get('update_time')

        time.sleep(1)


