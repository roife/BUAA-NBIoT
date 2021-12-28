<template>
  <base-nav
    container-classes="container-fluid"
    class="navbar-top navbar-expand"
    :class="{'navbar-dark': type === 'default'}"
  >
    <a href="#" aria-current="page" class="h4 mb-0 text-white text-uppercase d-none d-lg-inline-block active router-link-active"> {{$route.name}} </a>
    <!-- Navbar links -->
    <b-navbar-nav class="align-items-center ml-md-auto">
      <!-- This item dont have <b-nav-item> because item have data-action/data-target on tag <a>, wich we cant add -->
    </b-navbar-nav>
    <b-navbar-nav class="align-items-center ml-auto ml-md-0">
      <base-dropdown menu-on-right
                     class="nav-item"
                     tag="li"
                     title-tag="a"
                     title-classes="nav-link pr-0">
        <a href="#" class="nav-link pr-0" @click.prevent slot="title-container">
          <b-media no-body class="align-items-center">
            <b-media-body class="ml-2 d-none d-lg-block">
              <span class="mb-0 text-sm  font-weight-bold">{{ this.device_name === null ? "未知设备" : this.device_name }}</span>
            </b-media-body>
          </b-media>
        </a>

        <template>
          <b-dropdown-item href="#" @click="toggleWarning()">
            <i class="ni ni-bell-55"></i>
            <span>{{ this.record.motor === 0 ? "发送警报" : "停止警报" }}</span>
          </b-dropdown-item>

          <b-dropdown-item href="#" v-b-modal.modal-critical-temperature>
            <i class="ni ni-sound-wave"></i>
            <span>设置报警温度</span>
          </b-dropdown-item>

          <div class="dropdown-divider"></div>

          <b-dropdown-item href="#" v-b-modal.modal-register>
            <i class="ni ni-circle-08"></i>
            <span>绑定新设备</span>
          </b-dropdown-item>
        </template>

      </base-dropdown>
    </b-navbar-nav>

    <b-modal id="modal-register" hide-footer title="绑定新设备">
      <b-form @submit="onCriticalTemperatureSubmit">
        <b-form-group id="input-group-app_key" label="AppKey" label-for="input-app_key">
            <b-form-input
                id="input-app_key"
                v-model="form.app_key"
                placeholder="AppKey"
                required
            ></b-form-input>
        </b-form-group>

        <b-form-group id="input-group-app_secret" label="AppSecret" label-for="input-app_secret">
            <b-form-input
                id="input-app_secret"
                v-model="form.app_secret"
                placeholder="AppSecret"
                required
            ></b-form-input>
        </b-form-group>

        <b-form-group id="input-group-master_key" label="MasterKey" label-for="input-master_key">
            <b-form-input
                id="input-master_key"
                v-model="form.master_key"
                placeholder="MasterKey"
                required
            ></b-form-input>
        </b-form-group>

        <b-form-group id="input-group-product_id" label="ProductId" label-for="input-product_id">
            <b-form-input
                id="input-product_id"
                v-model="form.product_id"
                placeholder="ProductId"
                required
            ></b-form-input>
        </b-form-group>

        <b-form-group id="input-group-device_id" label="DeviceId" label-for="input-device_id">
            <b-form-input
                id="input-device_id"
                v-model="form.device_id"
                placeholder="DeviceId"
                required
            ></b-form-input>
        </b-form-group>
        <b-button type="submit" variant="primary">Submit</b-button>
      </b-form>
    </b-modal>

    <b-modal id="modal-critical-temperature" hide-footer title="设置报警温度">
      <b-form @submit="onSubmit">
        <b-form-group id="input-group-critical-temperature" label="报警温度" label-for="input-critical-temperature">
            <b-form-input
                id="input-critical-temperature"
                v-model="critical_temperature_form.critical_temperature"
                placeholder="50"
                required
            ></b-form-input>
        </b-form-group>

        <b-button type="submit" variant="primary">Submit</b-button>
      </b-form>
    </b-modal>

  </base-nav>
</template>
<script>
import { BaseNav, Modal } from '@/components';

// Axios
import axios from 'axios';

export default {
  components: {
    BaseNav,
    Modal
  },
  props: {
    type: {
      type: String,
      default: 'default', // default|light
      description: 'Look of the dashboard navbar. Default (Green) or light (gray)'
    }
  },
  computed: {
    routeName() {
      const { name } = this.$route;
      return this.capitalizeFirstLetter(name);
    }
  },
  mounted() {
    this.id = localStorage.getItem("id");
    this.app_key = localStorage.getItem("app_key");
    this.app_secret = localStorage.getItem("app_secret");
    this.master_key = localStorage.getItem("master_key");
    this.product_id = localStorage.getItem("product_id");
    this.device_id = localStorage.getItem("device_id");
    this.device_name = localStorage.getItem("device_name");

    this.timer = setInterval(this.fetchRecords, 1000);
  },
  data() {
    return {
      activeNotifications: false,
      showMenu: false,

      // register_form
      form: {
        app_key: '',
        app_secret: '',
        master_key: '',
        product_id: '',
        device_id: '',
      },

      critical_temperature_form: {
        critical_temperature: 50,
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
      record: '',

      // timer
      timer: '',
    };
  },
  methods: {
    capitalizeFirstLetter(string) {
      return string.charAt(0).toUpperCase() + string.slice(1);
    },
    toggleNotificationDropDown() {
      this.activeNotifications = !this.activeNotifications;
    },
    closeDropDown() {
      this.activeNotifications = false;
    },
    saveDeviceData() {
      localStorage.setItem("id", this.id);
      localStorage.setItem("app_key", this.app_key);
      localStorage.setItem("app_secret", this.app_secret);
      localStorage.setItem("master_key", this.master_key);
      localStorage.setItem("product_id", this.product_id);
      localStorage.setItem("device_id", this.device_id);
      localStorage.setItem("device_name", this.device_name);
    },
    onCriticalTemperatureSubmit() {
      let body = {
        critical_temperature: this.critical_temperature_form.critical_temperature,
      };
      axios.put(`${this.base_url}/devices/${this.id}/critical_temperature`, body)
        .then(_ => {
          window.location.reload();
        })
        .catch(err => console.log(err));
    },
    onSubmit() {
      let body = {
        app_key: this.form.app_key,
        app_secret: this.form.app_secret,
        master_key: this.form.master_key,
        product_id: this.form.product_id,
        device_id: this.form.device_id,
      };

      if (this.id !== null) {
        axios.delete(`${this.base_url}/devices/${this.id}`)
          .then(_ => {})
          .catch(err => console.log(err));
      }

      axios.post(`${this.base_url}/devices`, body)
        .then(res => {
          this.id = res.data.id;
          this.app_key = res.data.app_key;
          this.app_secret = res.data.app_secret;
          this.master_key = res.data.master_key;
          this.product_id = res.data.product_id;
          this.device_id = res.data.device_id;
          this.device_name = res.data.device_name;
          this.saveDeviceData();
          window.location.reload();
        })
        .catch(err => console.log(err));
    },
    toggleWarning() {
      if (this.id !== null) {
        if (this.record.motor === 0) {
          axios.post(`${this.base_url}/devices/${this.id}/warning`)
            .then(_ => {})
            .catch(err => console.log(err));
        } else {
          axios.delete(`${this.base_url}/devices/${this.id}/warning`)
            .then(_ => {})
            .catch(err => console.log(err));
        }
      }
    },
    fetchRecords() {
      if (this.id !== null) {
        axios.get(`${this.base_url}/devices/${this.id}/records`, { params: { "record_num": 1 } })
          .then(res => {
            this.record = res.data.at(0);
          })
          .catch(err => console.log(err));
      }
    }
  }
};
</script>
