def ms_to_min(ms):
    return ms/60000


def read_exceeded_sitting():
    file = open('numExceededSitting.txt')
    num = file.readline().strip()
    return int(num)


def read_dark_inside():
    file = open('numDarkIndoors.txt')
    num = file.readline().strip()
    return int(num)


def read_temp_light_vals(n):
    file = open('TempLightData.txt')
    times = []
    outdoor_temp_data = []
    indoor_temp_data = []
    light_data = []

    for line in file:
        line = line.strip()
        data = line.split(',')
        times.append(data[0])
        outdoor_temp_data.append(float(data[1]))
        indoor_temp_data.append(float(data[2]))
        light_data.append(float(data[3]))

    num_records = len(times)
    if num_records > n:
        times = times[num_records-n:]
        outdoor_temp_data = outdoor_temp_data[num_records-n:]
        indoor_temp_data = indoor_temp_data[num_records-n:]
        light_data = light_data[num_records-n:]

    return times, outdoor_temp_data, indoor_temp_data, light_data


def read_sitting_duration(n):
    file = open('sittingDuration.txt')
    times = []
    durations = []

    for line in file:
        line = line.strip()
        data = line.split(',')
        times.append(data[0])
        duration = ms_to_min(float(data[1]))
        durations.append(duration)

    num_records = len(times)
    if num_records > n:
        times = times[num_records - n:]
        durations = durations[num_records - n:]

    return times, durations


def get_percent_healthy_data(percent):
    file_append = open('percentHealthy.txt', 'a')
    file_append.write(str(percent) + '\n')

    percents = []

    file_read = open('percentHealthy.txt', 'r')
    for line in file_read:
        line = line.strip()
        percents.append(float(line))

    return percents
