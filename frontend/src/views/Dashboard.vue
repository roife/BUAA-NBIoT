<template>
  <div>
    <base-header class="pb-6 pb-7 pt-5 pt-md-8 bg-gradient-success" id="base-header">
      <!-- Card stats -->
      <b-row>
        <b-col xl="3" md="6">
          <stats-card title="实时温度"
                      type="gradient-red"
                      :sub-title="`${this.records && this.records[this.records.length - 1] ? this.records[this.records.length - 1].temperature.toFixed(2) : 0}℃`"
                      icon="thermometer"
                      class="mb-4">
          </stats-card>
        </b-col>
        <b-col xl="3" md="6">
          <stats-card title="实时湿度"
                      type="gradient-blue"
                      :sub-title="`${this.records && this.records[this.records.length - 1] ? this.records[this.records.length - 1].humidity.toFixed(2) : 0}%`"
                      icon="droplet"
                      class="mb-4">
          </stats-card>
        </b-col>
        <b-col xl="3" md="6">
          <stats-card title="马达状态"
                      type="gradient-orange"
                      :sub-title="(this.records && this.records[this.records.length - 1] ? this.records[this.records.length - 1].motor : 0) === 0 ? '未启动' : '启动'"
                      icon="phone-vibrate"
                      class="mb-4">
          </stats-card>
        </b-col>
        <b-col xl="3" md="6">
          <stats-card title="红外状态"
                      type="gradient-green"
                      :sub-title="(this.records && this.records[this.records.length - 1] ? this.records[this.records.length - 1].ir : 1) === 0 ? '遮挡' : '未遮挡'"
                      icon="hand-index"
                      class="mb-4">
          </stats-card>
        </b-col>
      </b-row>
    </base-header>

    <!--Charts-->
    <b-container fluid class="mt--7">
      <b-row>
        <b-col xl="8" class="mb-5 mb-xl-0">
          <card type="default" header-classes="bg-transparent">
            <b-row align-v="center" slot="header">
              <b-col>
                <h5 class="h3 text-white mb-0">近期数据</h5>
              </b-col>
              <b-col>
                <b-nav class="nav-pills justify-content-end">
                  <b-nav-item
                       class="mr-2 mr-md-0"
                       :active="bigLineChart.activeIndex === 0"
                       link-classes="py-2 px-3"
                       @click.prevent="initBigChart(0)">
                      <span class="d-none d-md-block">温度</span>
                      <span class="d-md-none">T</span>
                  </b-nav-item>
                  <b-nav-item
                    link-classes="py-2 px-3"
                    :active="bigLineChart.activeIndex === 1"
                    @click.prevent="initBigChart(1)"
                  >
                    <span class="d-none d-md-block">湿度</span>
                    <span class="d-md-none">H</span>
                  </b-nav-item>
                </b-nav>
              </b-col>
            </b-row>
            <line-chart
              :height="350"
              ref="bigChart"
              :chart-data="bigLineChart.chartData"
              :extra-options="bigLineChart.extraOptions"
            >
            </line-chart>
          </card>
        </b-col>
        <b-col xl="4" class="mb-5 mb-xl-0">
          <page-visits-table v-bind:tableData="records"></page-visits-table>
        </b-col>
      </b-row>
      <!-- End charts-->
    </b-container>
  </div>
</template>
<script>
  // Charts
  import * as chartConfigs from '@/components/Charts/config';
  import LineChart from '@/components/Charts/LineChart';
  import BarChart from '@/components/Charts/BarChart';

  // Components
  import BaseProgress from '@/components/BaseProgress';
  import StatsCard from '@/components/Cards/StatsCard';

  // Tables
  import PageVisitsTable from './Dashboard/PageVisitsTable';

  // Axios
  import axios from 'axios';

  export default {
    components: {
      LineChart,
      BarChart,
      BaseProgress,
      StatsCard,
      PageVisitsTable,
    },
    data() {
      return {
        bigLineChart: {
          activeIndex: 0,
          chartData: {},
          extraOptions: chartConfigs.blueChartOptions,
        },

        // API
        base_url: "http://49.232.173.163:19373",

        // device info
        id: 1,
        app_key: null,
        app_secret: null,
        master_key: null,
        product_id: null,
        device_id: null,
        device_name: null,

        // records
        records: [],

        // timer
        timer: '',
      };
    },
    methods: {
      initBigChart(index) {
        this.bigLineChart.activeIndex = index;
        this.updateChartData()
      },

      formatDate(dateStr) {
        var date = new Date(dateStr);
        var retStr = date.toLocaleString('en-us', { minute:"numeric", second:"numeric", hour12:false})
        return retStr
      },

      updateChartData() {
        let isTemp = this.bigLineChart.activeIndex === 0;
        let data = isTemp ? this.records.map(item => item.temperature) : this.records.map(item => item.humidity)
        let chartData = {
          datasets: [
            {
              label: isTemp ? "温度" : "湿度",
              data: data.map(item => item.toFixed(2))
            }
          ],
          labels: this.records.map(item => this.formatDate(item.created_at)),
        };

        this.bigLineChart.chartData = chartData;
      },

      fetchRecords() {
        if (this.id !== null) {
          axios.get(`${this.base_url}/devices/${this.id}/records`, { params: { "record_num": 40 } })
            .then(res => {
              this.records = res.data;
              this.updateChartData();
              if (this.records && this.records[this.records.length - 1].motor === 0) {
                document.getElementById("base-header").classList.remove("bg-gradient-warning");
                document.getElementById("base-header").classList.add("bg-gradient-success");
              } else {
                document.getElementById("base-header").classList.add("bg-gradient-warning");
                document.getElementById("base-header").classList.remove("bg-gradient-success");
              }
            })
            .catch(err => console.log(err));
        }
      }
    },
    mounted() {
      this.initBigChart(0);
      this.id = localStorage.getItem("id");
      this.app_key = localStorage.getItem("app_key");
      this.app_secret = localStorage.getItem("app_secret");
      this.master_key = localStorage.getItem("master_key");
      this.product_id = localStorage.getItem("product_id");
      this.device_id = localStorage.getItem("device_id");
      this.device_name = localStorage.getItem("device_name");

      this.timer = setInterval(this.fetchRecords, 1000);
    }
  };
</script>
<style>
.el-table .cell{
  padding-left: 0px;
  padding-right: 0px;
}
</style>
