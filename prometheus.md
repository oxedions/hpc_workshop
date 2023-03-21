# Prometheus ecosystem practical session

## 1. Prometheus

Open a first shell on server, using an ssh connection with 9090 local port forwarding (`ssh username@server -L 9090:localhost:9090`). Then on server, using wget command, download prometheus linux-amd64 binary from releases section on https://github.com/prometheus/prometheus (files are in "assets").

Extract archive using tar command (`tar xvzf file.tar.gz`). In extracted folder, you will see the prometheus static binary, and a configuration file sample prometheus.yml.

Launch manually prometheus using:

```
./prometheus --config.file prometheus.yml
```

Prometheus should start. Once started, let it run (you can stop it with a single Ctrl+c).

Since you connected with a port forwarding on port 9090, your local port is now linked to the server port. Open a web browser on your local system and go to `http://localhost:9090` . Prometheus interface should start.

Take the time to browse the interface. Have a special look on status:targets and status:configuration .
You will see that currently prometheus as a single target: himself (which we can disable if desired).

Let now this prometheus shell open and let prometheus running. We will come back to it later.

## 2. Node exporter

We now want to grab some data from our current node. To do so, we are going to use node_exporter. Remember that node_exporter exports a huge amount of data, and so this exporter should not be deployed on all nodes (only important ones) to avoid overloading prometheus.

Open a new ssh connection, this time with a local port forwarding on port 9100 (`-L 9100:localhost:9100`).

Then, on server, using wget, download latest node_exporter release from releases (linux-amd64) on https://github.com/prometheus/node_exporter/ .

Extract archive, and start node_exporter binary without any specific configuration (some configuration is available if needed, but for this training, default parameters are enough).

```
./node_exporter
```

Then, since we linked our local port 9100 to the same one on remote server, using your local browser, connect to http://localhost:9100/metrics . You will see all metrics provided by node_exporter running on the server.

Now we need to make prometheus aware of this exporter so it can pull data from it.

Let node_exporter running, and go back to prometheus shell, and stop prometheus (Ctrl+c). Then edit prometheus.yml configuration file, and add our node_exporter as a new target:

# A scrape configuration containing exactly one endpoint to scrape:
# Here it's Prometheus itself.
scrape_configs:
  - job_name: "node_exporter"
    static_configs:
      - targets: ["localhost:9100"]
  # The job name is added as a label `job=<job_name>` to any timeseries scraped from this config.
  - job_name: "prometheus"
    # metrics_path defaults to '/metrics'
    # scheme defaults to 'http'.
    static_configs:
      - targets: ["localhost:9090"]

Note that we are using `localhost:9100` here since node_exporter is running on the same server than prometheus (`localhost` means on the same system, it is equivalent to `127.0.0.1`). If we need to pull data from a remote server, we would replace localhost by the remote server ip or hostname.

And restart prometheus the same way than before. Now on your local browser, connect to prometheus as before (http://localhost:9090), and go into status:targets. Node exporter target should now be displayed here, and should be tagged as "UP" after few seconds. Prometheus will now pull data from this exporter every 15s (default value, can be changed in configuration file).

Wait 1-2 minutes, and go into graph tab. Ask for a table first, and request latest value of `node_network_transmit_bytes_total` and click execute to display it. You should see the current total value for each network interfaces your server posses. You can also ask for `node_load1` value for example. You can browse node_exporter page (http://localhost:9100/metrics) to see all values provided by node_exporter, with comments.

Now ask for `node_load1` or any other value, and ask for a graph instead of a table. You should see a graphic of your value over time. Prometheus graphics are made for system administrator. We will see more shiny graphics with grafana later.

## 3. Alertmanager

We now want to create some alerts, and we want to manage these alerts using Alertmanager. Alertmanager job is to pack alerts and send emails/sms/etc to system administrator(s). For example, if your whole cluster go down, you do not want to receive 1000 alerts (one per node if you have 1000 nodes). Alertmanager job is to pack all these alerts and send system administrator a single email with a summary.

Open a new shell, and connect to server using this time `-L 9093:localhost:9093`.

Once connected, download latest Alertmanager binary from https://github.com/prometheus/alertmanager/ .

Extract the archive, and you will find inside extracted folder the alertmanager static binary and a sample configuration file, which will be enough for this training.

Launch now alertmanager using:

```
./alertmanager --config.file alertmanager.yml 
```

You can reach alertmanager web interface on your http://localhost:9093 .

Right now, alertmanager will not report any alerts, since we haven't created any alerts for prometheus, and we haven't made prometheus aware of alertmanager.

### 3.1. Create new alert

Let's create a new alert that will check if all exporters are up and reachable. (so if our node_exporter go down, alert will trigger).

In prometheus shell, stop prometheus, and at the same place, create a file called my_rules.yml with the following content:

```yaml
groups:
  - name: General alerts
    rules:
      - alert: Exporter_down
        expr: up == 0
        for: 1m
        labels:
          severity: critical
        annotations:
          summary: "Exporter down (instance {{ $labels.instance }})"
          description: "Prometheus exporter down\n  VALUE = {{ $value }}\n  LABELS: {{ $labels }}"
```

This means that if for any exporter, up value is equal to 0 (up is a value you can get in graph table of prometheus interface), then if this happen during more than 1 minute, then prometheus should consider it as a critical alert.

Now open prometheus.yml file, and register this new rules file:

```yaml
# Load rules once and periodically evaluate them according to the global 'evaluation_interval'.
rule_files:
  - "my_rules.yml"
```

We now need to make prometheus aware of alertmanager.

### 3.2. Send alerts to alertmanager

Edit again prometheus.yml file, and ask prometheus to send alerts that trigger (fire) to our local alertmanager at port 9093:

```yaml
# Alertmanager configuration
alerting:
  alertmanagers:
    - static_configs:
        - targets:
          - localhost:9093
```

You can see that we can define multiple alertmanager instances for redundancy.

Now restart prometheus, and browse interface. In Alerts tab, you should now see your new alert. It should be (0 active) since our node_exporter is running.

### 3.3. Trigger an alert

Go into your node_exporter shell, and stop node_exporter (Ctrl+c). Then wait 1-2 minute and refresh Alerts page or prometheus web interface. You will see that alert status went to "pending", which means it is now waiting 1 minute more, and if after 1 minute exporter is still down, the alert will be triggered. Wait again 1-2 minutes, refresh, and you will see alert is now in red, which means it triggered. You can expand the alert to get more details on what is going on.

Now, connect to alertmanager interface (http://localhost:9093) and you will be able to see the alert. We haven't configured alertmanager to send alerts via mails, so alertmanager will only display sorted alerts (alerts are sorted by groups for convenient reading). Alertmanager interface is not very user friendly for day to day usage as alertmanager job is more focused on packing and sending alerts, we will see Karma later to improve that.

## 4. Karma

Karma is an optional tool, but useful for day to day monitoring. Karma allows to organize alerts on a dashboard, to be displayed on a permanent screen in system administrator's room.

Open a new terminal and ssh using `-L 8080:localhost:8080`.

Download karma binary from https://github.com/prymitive/karma, extract archive, and launch karma without a configuration file:

```
./karma-linux-amd64 --alertmanager.uri http://localhost:9093
```

And then reach your http://localhost:8080 to reach Karma web interface.
You should see the alert reported by prometheus via alertmanager. Feel free to use filters to sort issues, organize panel, etc.

Note that Karma can accept a configuration file, to customize interface, setup identification, etc. You can report to karma documentation on how to create this configuration file.

## 5. Grafana

Grafana allows to create nice day to day dashboards to visualize metrics.

Open a new terminal, and this time connect on server using `-L 3000:localhost:3000`.

Then download grafana OSS (be sure to take the OSS version, not the enterprise version) from https://grafana.com/grafana/download?edition=oss

Url should be similar to https://dl.grafana.com/oss/release/grafana-9.4.3.linux-amd64.tar.gz.

Once downloaded, extract archive, and inside extracted folder, go into bin folder, and launch grafana server this way:

```
./grafana-server
```

Now connect to your http://localhost:3000 on local web browser, and uses admin / admin as credentials to connect. You will be asked to update password, keep it "admin" for this training.

Once inside grafana interface, add a new source, and request a Prometheus source, and use http://localhost:9090 URL as source endpoint. Validate and test it.

Once source has been setup, create a new dashboard, and add a new panel on it.

Use our prometheus as source, and try to use `node_load1` as query to display it. You can then customize legend, title, etc. Add other elements to dashboard, etc. Remember to save your changes on the top right corner every time.

If you want to go further, a nide tutorial is available at https://grafana.com/tutorials/grafana-fundamentals/ .


## 6. Custom exporter

Open another tunnel and bind port 8777 to your local host at the same time: `-L 8777:localhost:8777`.

Install now Prometheus Client python modules:

```
pip3 install prometheus-client
```

And create a custom exporter to monitor a specific command output. We will try to parse the output of `df` command to get system root file system usage in percent.

To do so, we are going to use the following pipeline command: `df -h | grep '/$' | awk -F ' ' '{print $5}' | sed 's/%//'`

Create a file **my_exporter.py** with the following content:

```python
#!/usr/bin/env python3
# This is a minimal exporter, to be used as a reference.

import time
import subprocess
import sys
from prometheus_client.core import GaugeMetricFamily, REGISTRY
from prometheus_client import start_http_server


class CustomCollector(object):
    def __init__(self):
        pass

    def collect(self):
        try:
            stdout, stderr = subprocess.Popen("df -h | grep '/$' | awk -F ' ' '{print $5}' | sed 's/%//'", stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True).communicate()
        except OSError as e:
            print("Execution failed:", e, file=sys.stderr)
        print("Get root FS usage: " + str(stdout))
        g = GaugeMetricFamily("root_fs_percent", 'Root FS usage in percent')
        g.add_metric(["root_fs_percent"], int(stdout))
        yield g


if __name__ == '__main__':
    start_http_server(8777)
    REGISTRY.register(CustomCollector())
    while True:
        time.sleep(1)
```

Make this file executable, and launch the exporter:

```
chmod +x my_exporter.py
./my_exporter.py
```

You can now browse to http://localhost:8777 and see metrics provided by the exporter. At the very bottom, you should see your FS usage in percent.

Try now to add this exporter into prometheus main configuration so prometheus pulls it, and add a new gauge item into grafana dashboard with your FS usage.
