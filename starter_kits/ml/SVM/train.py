#!/usr/bin/env python3

import model

m = model.HaliteModel()
m.train_on_files('training', 'aggressive')
m.save(file_name='aggressive.svc')

m = model.HaliteModel()
m.train_on_files('training', 'passive')
m.save(file_name='passive.svc')
