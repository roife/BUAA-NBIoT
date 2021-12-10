require 'openssl'
require 'base64'
require 'net/http'
require 'json'

module NBIoTHelper
    def signature(key, application, timestamp, params, body)
        code = "application:" + application + "\n" + "timestamp:" + timestamp + "\n"

        params.each {|key, val| code += key.to_s + ":" + val.to_s + "\n"}

        unless body.nil? || body.empty?
        code += body.to_json + "\n"
        end

        digest = OpenSSL::Digest.new('sha1')
        sig = OpenSSL::HMAC.digest(digest, key, code)
        return Base64::encode64(sig)
    end

    def send_request(path, head, params, body, version, application, master_key, key, method)
        base_url = 'http://ag-api.ctwing.cn'
        url = base_url + path

        params.update({:MasterKey => master_key}) unless master_key.nil? || master_key.strip.empty?
        params = params.sort_by{|key, _| key}.to_h

        timestamp = (Time.now.to_f * 1000).to_i.to_s

        headers = {
            :application => application,
            :Date => Time.now.to_s,
            :timestamp => timestamp,
            :version => version,
            :signature => signature(key, application, timestamp, params, body)
        }
        headers.update({:MasterKey => master_key}) unless master_key.nil? || master_key.strip.empty?
        headers.update(head)

        uri = URI(url)
        uri.query = URI.encode_www_form(params)
        if method == "GET"
            req = Net::HTTP::Get.new(uri, initheader = headers)
            res = Net::HTTP.start(uri.hostname, uri.port) {|http|
                http.request(req)
            }
            puts "Response #{res.code} #{res.message}: #{res.body}"
            return res
        elsif method == "POST"
            req = Net::HTTP::Post.new(uri.request_uri, initheader = headers)
            req.body = body.to_json
            res = Net::HTTP.start(uri.hostname, uri.port) do |http|
                http.request(req)
            end
            puts "Response #{res.code} #{res.message}: #{res.body}"
            return res
        end
    end

    def query_device_status_list(app_key, app_secret, body)
        path = '/aep_device_status/deviceStatusList'
        head = {}
        params = {}
        version = '20181031202403'
        application = app_key
        key = app_secret

        res = send_request(path, head, params, body, version, application, nil, key, "POST")
        return res
    end

    def query_device_list(app_key, app_secret, master_key, product_id, search_value = "", page_now = 0, pageSize = 0)
        path = '/aep_device_management/devices'
        head = {}
        params = {:productId => product_id, :searchValue => search_value, :pageNow => page_now, :pageSize => pageSize}
        version = '20190507012134'
        application = app_key
        key = app_secret

        res = send_request(path, head, params, nil, version, application, master_key, key, "GET")
        return res
    end

    def create_command(app_key, app_secret, master_key, body)
        path = '/aep_device_command/command'
        head = {}
        param = {}
        version = '20190712225145'
        application = app_key
        key = app_secret

        res = send_request(path, head, param, body, version, application, master_key, key, "POST")
        return res
    end

    def analysis_status(status)
        status = status.map{|item| {item[:datasetId].to_sym => item}}.inject(:update)

        ret = {}

        ret[:temperature] = status[:temperature_data][:value]
        ret[:humidity] = status[:humidity_data][:value]

        sensor_data = Base64.decode64(status[:sensor_data][:value])
        ret[:ir] = sensor_data[0].ord
        ret[:led0] = sensor_data[1].ord
        ret[:motor] = sensor_data[2].ord

        return ret
    end

    def query_name(app_key, app_secret, master_key, product_id, device_id)
        res = NBIoTHelper.query_device_list(app_key, app_secret, master_key, product_id)
        res = JSON.parse(res.body, symbolize_names: true)[:result][:list]
                .find{|device| device[:deviceId] == device_id}
        return res ? res[:deviceName] : nil
    end

    module_function :signature
    module_function :send_request
    module_function :query_device_list
    module_function :query_device_status_list
    module_function :create_command
    module_function :analysis_status
    module_function :query_name
end
