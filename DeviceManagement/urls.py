from django.urls import path
from . import views

urlpatterns = [
    path('get_latest_record/<str:device_id>/', views.get_latest_record, name='get_latest_record'),
    path('device/<str:device_id>/', views.device_detail, name='device_detail'),
    path('get_history_records/', views.get_history_records, name='get_history_records'),
    path('', views.dashboard, name='dashboard'),
    path('get_unique_device_records/', views.get_unique_device_records, name='get_unique_device_records'),
]

