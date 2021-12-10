Rails.application.routes.draw do
  resources :records, only: [:show]
  resources :devices, only: [:show, :create, :destroy]

  get 'devices/:id/records', to: 'devices#show_records'
  post 'devices/:id/warning', to: 'devices#start_warning'
  delete 'devices/:id/warning', to: 'devices#stop_warning'
  # For details on the DSL available within this file, see http://guides.rubyonrails.org/routing.html
end
