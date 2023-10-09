from django.db.models import Max
from django.http import JsonResponse
from .models import LoraMessage
from django.shortcuts import render
import json


def get_latest_record_data(device_id):
    try:
        latest_record = LoraMessage.objects.filter(device_id=device_id).latest('update_time')

        return latest_record.to_dict()
    except LoraMessage.DoesNotExist:
        return None


def get_latest_record(request, device_id):
    data = get_latest_record_data(device_id)
    if data:
        return JsonResponse({'data': data}, safe=False)
    return JsonResponse({"error": "No record found for the specified device ID"}, status=404)


def get_history_records(request, device_id):
    all_records = LoraMessage.objects.filter(device_id=device_id).order_by('update_time')[1:]

    data_list = [record.to_dict() for record in all_records]

    if data_list:
        return JsonResponse({'data': data_list}, safe=False)
    return JsonResponse({"error": "No records found"}, status=404)


def device_info(request):
    return render(request, 'device_details.html')


def dashboard(request):
    return render(request, 'dashboard.html')


def get_unique_device_records(request):
    latest_update_times = (
        LoraMessage.objects
        .values('device_id')
        .annotate(max_update_time=Max('update_time'))
    )

    #
    latest_device_records = LoraMessage.objects.filter(
        update_time__in=[device['max_update_time'] for device in latest_update_times]
    )

    serialized_data = [
        {
            'device_id': record.device_id,
            'location': record.location,
            'brightness': record.brightness,
            'update_time': record.update_time,
        }
        for record in latest_device_records
    ]

    # 创建一个包含最新记录的字典
    response_data = {'latest_device_records': serialized_data}

    # 使用JsonResponse返回JSON响应
    return JsonResponse(response_data, safe=False)


def device_detail(request, device_id):
    data = get_latest_record_data(device_id)
    all_records = LoraMessage.objects.filter(device_id=device_id).order_by('-update_time')[1:]
    hist_data = [record.to_dict() for record in all_records]

    if data:
        return render(request, 'device_details.html', {'data': data, 'hist_data': hist_data})
    return render(request, 'error_404.html')
