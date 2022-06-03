from flask import Flask, render_template
from flask import request
from read_files_helper import read_exceeded_sitting, read_dark_inside, read_temp_light_vals, \
    read_sitting_duration, get_percent_healthy_data
import matplotlib.pyplot as plt
from io import BytesIO
import base64

app = Flask(__name__)

HEALTHY_NUM_SITTING = 5
HEALTHY_NUM_DARK = 1
NUM_DATA = 15

def get_mean_data(data, is_two_plots=False):
    mean = sum(data) / len(data)
    if is_two_plots:
        return [mean] * (len(data) // 2)
    return [mean] * len(data)


def get_plot_url(data, mean_data, title, xlabel, ylabel, x_axis_vals, data_label, data_label2=None, two_plots=False,
                 data2=None, xdate=True):
    img = BytesIO()

    plt.plot_date(x_axis_vals, data, linestyle='solid', xdate=xdate)
    plt.plot(mean_data, '--')
    if two_plots:
        plt.plot(data2, '-bo')

    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.xticks(rotation=35)
    plt.legend([data_label, 'mean', data_label2])

    plt.savefig(img, format='png')
    plt.close()
    img.seek(0)
    plot_url = base64.b64encode(img.getvalue()).decode('utf8')

    return plot_url


def calculate_percent_healthy(num_exceeded_sitting, num_dark_inside):
    sitting_difference = num_exceeded_sitting - HEALTHY_NUM_SITTING
    dark_inside_difference = num_dark_inside - HEALTHY_NUM_DARK

    if sitting_difference <= 0 and dark_inside_difference <= 0:
        return 100
    elif sitting_difference > 0 and dark_inside_difference > 0:
        percent = 100 - (sitting_difference * 3 + dark_inside_difference * 4)
    elif sitting_difference > 0:
        percent = 100 - (sitting_difference * 3)
    elif dark_inside_difference > 0:
        percent = 10 - (dark_inside_difference * 4)
    
    if percent < 0:
        return 0
    return percent

def get_aggregation(data, mean_data):
    aggregation = {}
    if data:
        aggregation['min'] = round(min(data), 2)
        aggregation['max'] = round(max(data), 2)
    if mean_data:
        aggregation['mean'] = round(mean_data[0], 2)

    return aggregation


@app.route("/")
def dashboard():
    num_exceeded_sitting = read_exceeded_sitting()
    num_dark_inside = read_dark_inside()
    temp_light_times, outdoor_temp, indoor_temp, light_values = read_temp_light_vals(NUM_DATA)
    sitting_times, sitting_duration = read_sitting_duration(NUM_DATA)

    longest_sitting_duration = max(sitting_duration)
    percent_healthy = calculate_percent_healthy(num_exceeded_sitting, num_dark_inside)
    percent_healthy_data = get_percent_healthy_data(percent_healthy)

    mean_temp_vals = get_mean_data(outdoor_temp + indoor_temp, True)
    mean_light_vals = get_mean_data(light_values)
    mean_sitting_vals = get_mean_data(sitting_duration)
    mean_percent_healthy = get_mean_data(percent_healthy_data)

    sitting_duration_plot = get_plot_url(sitting_duration,
                                         mean_sitting_vals,
                                         'Sitting Durations',
                                         'Time',
                                         'Duration (mins)',
                                         sitting_times,
                                         'duration')
    temp_plot = get_plot_url(indoor_temp,
                             mean_temp_vals,
                             'Indoor vs Outdoor Temperature',
                             'Time',
                             'Temp (F)',
                             temp_light_times,
                             'indoor temp',
                             'outdoor temp',
                             True,
                             outdoor_temp)
    light_plot = get_plot_url(light_values,
                              mean_light_vals,
                              'Light values',
                              'Time',
                              'On a scale of 0-100',
                              temp_light_times,
                              'light')
    percent_healthy_plot = get_plot_url(percent_healthy_data,
                                        mean_percent_healthy,
                                        'Healthiness Trends',
                                        'Time',
                                        'Percentage',
                                        [i for i in range(len(percent_healthy_data))],
                                        'healthy',
                                        xdate=False)

    sitting_aggregation = get_aggregation(sitting_duration, mean_sitting_vals)
    temp_aggregation = get_aggregation(outdoor_temp + indoor_temp, mean_temp_vals)
    light_aggregation = get_aggregation(light_values, mean_light_vals)
    health_aggregation = get_aggregation(percent_healthy_data, mean_percent_healthy)

    return render_template("dashboard.html", numExceededSitting=num_exceeded_sitting, numDarkInside=num_dark_inside,
                           longestSittingDuration=longest_sitting_duration, percentHealthy=percent_healthy,
                           sittingPlot=sitting_duration_plot, tempPlot=temp_plot, lightPlot=light_plot,
                           sittingAggreggation=sitting_aggregation, tempAggregation=temp_aggregation,
                           lightAggregation=light_aggregation, percentHealthyPlot=percent_healthy_plot,
                           healthAggregation=health_aggregation)


@app.route("/temp-light-sensor", methods = ["POST"])
def store_temp_light_data():
    req = request.form.to_dict()
    print(req)

    dataFile = open("TempLightData.txt", "a")
    stringToWrite = f'{req["time"]},{req["outdoorTemp"]},{req["indoorTemp"]},{req["indoorLight"]}'
    dataFile.write(stringToWrite+"\n")
    dataFile.close()

    return f"WROTE TO FILE: {stringToWrite}"

@app.route("/num-exceeded-sitting", methods = ["POST"])
def store_num_exceeded_data():
    req = request.form.to_dict()
    print(req)

    dataFile = open("numExceededSitting.txt", "w")
    stringToWrite = str(req["numExceededSitting"])
    dataFile.write(stringToWrite+"\n")
    dataFile.close()

    return f"WROTE TO FILE: {stringToWrite}"

@app.route("/sitting-duration", methods = ["POST"])
def store_sitting_duration():
    req = request.form.to_dict()
    print(req)

    dataFile = open("sittingDuration.txt", "a")
    stringToWrite = f'{req["time"]},{req["sittingDuration"]}'
    dataFile.write(stringToWrite+"\n")
    dataFile.close()

    return f"WROTE TO FILE: {stringToWrite}"

@app.route("/dark-indoors", methods = ["POST"])
def store_num_dark_indoors():
    req = request.form.to_dict()
    print(req)

    dataFile = open("numDarkIndoors.txt", "w")
    stringToWrite = str(req["numDarkIndoors"])
    dataFile.write(stringToWrite+"\n")
    dataFile.close()

    return f"WROTE TO FILE: {stringToWrite}"

@app.errorhandler(404)
def handle_error(e):
    return request.url
