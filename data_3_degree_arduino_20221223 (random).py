# Shuffle & Split
#https://jonsyou.tistory.com/23

# Accuracy
#https://steadiness-193.tistory.com/277

# SVR
#https://github.com/mk-gurucharan/Regression/blob/master/Models/Support_Vector_Regression.ipynb
#https://creatorjo.tistory.com/141?category=883823
#https://creatorjo.tistory.com/138

#아두이노 통신
#import serial
#import time

#SVR
import numpy as np
from sklearn.svm import SVR
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import openpyxl
import math
from sklearn.model_selection import ShuffleSplit
from sklearn.model_selection import cross_validate
from sklearn.model_selection import GridSearchCV
from sklearn.model_selection import cross_val_score
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score, mean_absolute_error, mean_squared_error, mean_squared_log_error, r2_score
from sklearn.model_selection import validation_curve

# #############################################################################
# Generate sample data
data = pd.read_excel("correct_data.xlsx", header=0)
newData = data[['angle', 'PWM']]

#속성(변수) 선택
x = newData[['angle']]
y = newData[['PWM']]

# #############################################################################
# Splitting the dataset into the Training set and Test set
#x_test,y_test : testing data, 20%
#x_train, y_train : training data, 80%
model = SVR(kernel = 'rbf', C=100, gamma=0.03, epsilon=5, degree=4, coef0=1)

#ShuffleSplit
ss = ShuffleSplit(n_splits=3, test_size=0.2, random_state=120)

n_iter = 0

for train_index, test_index in ss.split(newData):

    n_iter += 1
    train = newData.iloc[train_index]
    test = newData.iloc[test_index]

    print("==============================")
    print("교차검증 번호: {0}".format(n_iter))

    print("학습 데이터")
    print(train)
    print("검증 데이터")
    print(test)
    print("==============================")

    x_train = train[['angle']]
    y_train = train[['PWM']]
    x_test = test[['angle']]
    y_test = test[['PWM']]
    
    #train accuracy
    model.fit(x_train, y_train.values.ravel())#.reshape(-1,1)
    pred = model.predict(x_train)

    dt_clf = DecisionTreeClassifier()
    dt_clf.fit(x_train, y_train)
    pred = dt_clf.predict(x_train)
    print(pred)
    accuracy = accuracy_score(y_train, pred)
    print('예측 정확도 : {0:.4f}'.format(accuracy))
    print('\n')

    #relation_square = model.score(x_test, y_test)
    print('R2_score : {:.3f} ', format(r2_score(y_train, pred)))
    print('RMSE     : {:.6f} ', format(np.sqrt(mean_squared_error(y_train, pred))))
    #print('MAE      : {:.6f} ', format(mean_absolute_error(y_train, pred)))
    #print('MSE      : {:.6f} ', format(mean_squared_error(y_train, pred)))
  
# #############################################################################
# Visualising the SVR results (for higher resolution and smoother curve)
x_grid = np.arange(6, 21.01, 0.01)
x_grid = x_grid.reshape((len(x_grid), 1))

# #############################################################################
# Crossvalidation
#for i in ('linear', 'poly', 'rbf'): #rbf excel data 뽑고 싶으면 뒤에 poly 지울 것 'linear', 'rbf', 'poly'
model = SVR(kernel = 'rbf', C=100, gamma=0.03, epsilon=5, degree=4, coef0=1)

cv_results = cross_validate(model, x_train, y_train.values.ravel(), cv=ss)
df = pd.DataFrame(cv_results)
df = df.sort_values(by='test_score', ascending=False)
#fit_time(모델훈련시간), score_time(모델검증시간), test_score(교차검증의 최종 점수)
print(df)
print('\n')

#RBF 교차검증의 최종점수 평균
print("교차검증 최종점수 평균")
print(cv_results['test_score'].mean())
print('\n')

# ***96%, C=100, gamma=0.03, epsilon=5, degree=3, coef0=1, random_state=120, 'rbf'
# 95.7%, C=100, gamma=0.1, epsilon=5, degree=3, coef0=1, random_state=9, 'rbf'


# #############################################################################


#Optimal Hyperparameter
#Grid Search 수행
reg = GridSearchCV(estimator=model, param_grid={
            'C': [50,80,100,120,150,200],
            'epsilon': [1,3,5,7,10],
            'gamma': [0.01,0.02,0.03,0.04,0.05],
            'coef0': [1],
            'degree': [1]
        },
        cv=ss, scoring='neg_mean_squared_error', verbose=0, n_jobs=-1)
grid_result = reg.fit(x_train, y_train.values.ravel())
best_params = grid_result.best_params_

best_svr = SVR(kernel='rbf', C=best_params["C"], epsilon=best_params["epsilon"], gamma=best_params["gamma"],
                   coef0=best_params["coef0"],degree=best_params["degree"], shrinking=True,
                   tol=0.001, cache_size=200, verbose=False, max_iter=-1)
print(best_svr)


# #############################################################################
# Look at the results
for i in ('linear', 'poly', 'rbf'): #rbf data 뽑고 싶으면 뒤에 poly 지울 것 'linear', 'rbf', 'poly'
  model = SVR(kernel = i, C=100, gamma=0.03, epsilon=5, degree=4, coef0=1)
  model.fit(x_train, y_train.values.ravel())#.reshape(-1,1)
  
  y_train_p = model.predict(x_train)
  y_test_p = model.predict(x_test)

  #틀림 relation_square = model.score(x_test, y_test)
  print('R2_score : {:.3f} ', format(r2_score(y_train.values.ravel(), y_train_p)))

  #plt.xlim(15,30)
  #plt.ylim(0,255)
  plt.scatter(x_train, y_train_p, marker = 'o', color = 'red') #80%
  plt.scatter(x_test, y_test_p, marker = 'o', color = 'green') #20%
  plt.plot(x_grid, model.predict(x_grid), color = 'black')

  plt.title('SVR Regression')
  plt.xlabel('angle')
  plt.ylabel('PWM')
  plt.show()
  plt.close()

# #############################################################################
# 엑셀에 예측값(list) 저장

angle = x_grid.reshape((1502,))
pwm = model.predict(x_grid).reshape((1502,))

angle_list = pd.Series(angle, name = 'angle')
pwm_list = pd.Series(pwm, name = 'PWM')
list = pd.concat([angle_list, pwm_list], axis = 1)
#pwm_ard = str(list.iloc[[150],[1]]) #3은 index를 가르킴
#print(pwm_ard)

list.to_excel('SVR_traing_data.xlsx', index=False)


