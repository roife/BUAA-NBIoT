<template>
  <b-card body-class="p-0" header-class="border-0">
    <template v-slot:header>
      <b-row align-v="center">
        <b-col>
          <h3 class="mb-0">近期数据</h3>
        </b-col>
        <b-col class="text-right">
          <a href="#" v-b-modal.modal-all-data class="btn btn-sm btn-primary">查看全部</a>
        </b-col>
      </b-row>
    </template>

    <el-table class="table-responsive table"
              :data="tableData.slice(-7).reverse()">
      <el-table-column label="时间"
                       min-width="130px"
                       prop="created_at">
        <template v-slot="{row}">
          <div class="font-weight-600">{{formatDate(row.created_at)}}</div>
        </template>
      </el-table-column>
      <el-table-column label="温度"
                       min-width="70px"
                       prop="temperature">
        <template v-slot="{row}">
          <div>{{row.temperature.toFixed(2)}}</div>
        </template>
      </el-table-column>
      <el-table-column label="湿度"
                  min-width="70px"
                  prop="humidity">
        <template v-slot="{row}">
          <div>{{row.humidity.toFixed(2)}}</div>
        </template>
      </el-table-column>
    </el-table>

    <b-modal id="modal-all-data" title="全部数据">
      <el-table class="table-responsive table"
              :data="tableData.slice().reverse()">
        <el-table-column label="时间"
                        min-width="130px"
                        prop="created_at">
          <template v-slot="{row}">
            <div class="font-weight-600">{{formatDate(row.created_at)}}</div>
          </template>
        </el-table-column>
        <el-table-column label="温度"
                        min-width="70px"
                        prop="temperature">
          <template v-slot="{row}">
            <div>{{row.temperature.toFixed(2)}}</div>
          </template>
        </el-table-column>
        <el-table-column label="湿度"
                    min-width="70px"
                    prop="humidity">
          <template v-slot="{row}">
            <div>{{row.humidity.toFixed(2)}}</div>
          </template>
        </el-table-column>
      </el-table>
    </b-modal>
  </b-card>
</template>
<script>
  import { Table, TableColumn, DropdownMenu, DropdownItem, Dropdown} from 'element-ui'
  export default {
    name: 'page-visits-table',
    components: {
      [Table.name]: Table,
      [TableColumn.name]: TableColumn,
      [Dropdown.name]: Dropdown,
      [DropdownItem.name]: DropdownItem,
      [DropdownMenu.name]: DropdownMenu,
    },
    methods: {
      formatDate(dateStr) {
        var date = new Date(dateStr);
        var retStr = date.toLocaleString('en-us', { month:"short", day:"numeric", hour:"numeric", minute:"numeric", second:"numeric", hour12:false})
        return retStr
      }
    },
    data() {
      return {

      }
    },
    props:["tableData"]
  }
</script>
<style>
</style>
