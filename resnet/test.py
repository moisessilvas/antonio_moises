import tensorflow as tf

from tensorflow.keras import datasets, layers, models
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.callbacks import ModelCheckpoint, CSVLogger
import matplotlib.pyplot as plt
import cv2 as cv

tf.keras.backend.set_floatx('float64')
'''
image = cv.imread("/home/antonio/Imagens/gray/g/Grayscale2.jpg")
gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
print(gray.shape)
#cv.imwrite("/home/antonio/Imagens/gray/g/Grayscale2.jpg", gray)
'''
datagen = ImageDataGenerator(
    validation_split=0.2)

outgen = datagen.flow_from_directory(
    directory="/home/antonio/Imagens/gray/",
    class_mode='binary',
    color_mode='grayscale',
    batch_size=1,
    target_size=(224,224))

traingen = datagen.flow_from_directory(
    #directory="/home/antonio/Imagens/gray/",
    #directory="/home/antonio/Downloads/dog vs cat/dataset/test_set/",
    directory="/home/antonio/Imagens/labAdr/",
    class_mode='binary',
    color_mode='grayscale',
    batch_size=1,
    subset='training',
    target_size=(224,224))

testgen = datagen.flow_from_directory(
    #directory="/home/antonio/Downloads/dog vs cat/dataset/test_set/",
    directory="/home/antonio/Imagens/labAdr/",
    class_mode='binary',
    color_mode='grayscale',
    batch_size=1,
    subset='validation',
    target_size=(224,224))

inputs = layers.Input(shape=(224, 224, 1))

# Layer 1
x = layers.ZeroPadding2D((3,3))(inputs)
x = layers.Conv2D(64, (7, 7), (2, 2), padding='valid', use_bias=False, name='conv1')(x)

# Layer 2
x = layers.ZeroPadding2D((1,1))(x)
x1 = layers.MaxPooling2D((3, 3), strides=(2,2), padding='valid')(x)
x = layers.ZeroPadding2D((1,1))(x1)
x = layers.Conv2D(64, (3, 3), padding='valid', use_bias=False, name='conv2_1a')(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(64, (3, 3), padding='valid', use_bias=False, name='conv2_2a')(x)
added = layers.add([x1,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(64, (3, 3), padding='valid', use_bias=False, name='conv2_1b')(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(64, (3, 3), padding='valid', use_bias=False, name='conv2_2b')(x)
added = layers.add([added,x3])

# Layer 3

sc = layers.Conv2D(128, (1, 1), (2, 2), padding='valid', use_bias=False, name='shortcut3')(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(128, (3, 3), (2, 2), padding='valid', use_bias=False, name='conv3_1a')(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(128, (3, 3), padding='valid', use_bias=False, name='conv3_2a')(x)
added = layers.add([sc,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(128, (3, 3), padding='valid', use_bias=False, name='conv3_1b')(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(128, (3, 3), padding='valid', use_bias=False, name='conv3_2b')(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

# Layer 4

sc = layers.Conv2D(256, (1, 1), (2, 2), padding='valid', use_bias=False, name='shortcut4')(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(256, (3, 3), (2, 2), padding='valid', use_bias=False, name='conv4_1a')(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(256, (3, 3), padding='valid', use_bias=False, name='conv4_2a')(x)
added = layers.add([sc,x2])

x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(256, (3, 3), padding='valid', use_bias=False, name='conv4_1b')(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(256, (3, 3), padding='valid', use_bias=False, name='conv4_2b')(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

# Layer 5

sc = layers.Conv2D(512, (1, 1), (2, 2), padding='valid', use_bias=False, name='shortcut5')(added)
x = layers.ZeroPadding2D((1, 1))(added)
x = layers.Conv2D(512, (3, 3), (2, 2), padding='valid', use_bias=False, name='conv5_1a')(x)
x = layers.ZeroPadding2D((1,1))(x)
x2 = layers.Conv2D(512, (3, 3), padding='valid', use_bias=False, name='conv5_2a')(x)
added = layers.add([sc,x2])
x = layers.ZeroPadding2D((1,1))(added)
x = layers.Conv2D(512, (3, 3), padding='valid', use_bias=False, name='conv5_1b')(x)
x = layers.ZeroPadding2D((1,1))(x)
x3 = layers.Conv2D(512, (3, 3), padding='valid', use_bias=False, name='conv5_2b')(x)
added = layers.add([added,x3])
x = layers.ZeroPadding2D((1,1))(added)

#flat = layers.Flatten()(added)
#output = layers.Dense(1, use_bias=False)(flat)
output = layers.AveragePooling2D((7,7))(added)
flat = layers.Flatten()(output)
result = layers.Dense(1, use_bias=False)(flat)
#result = layers.Activation("sigmoid")(output)
model = models.Model(inputs=inputs, outputs=result)

model_filepath = "results/modelbw-{epoch:04d}.h5"

model_checkpoint = ModelCheckpoint(
    filepath=model_filepath,
    verbose=1,
    period=1)

csv_logger = CSVLogger("logs/test.csv")

model.compile(loss="binary_crossentropy", optimizer='adam', metrics=['accuracy'])
#model.fit(traingen, epochs=1, steps_per_epoch=1)
model.fit(
    traingen,
    callbacks=[model_checkpoint, csv_logger],
    validation_data=testgen,
    validation_steps=testgen.samples//1,
    epochs=10,
    steps_per_epoch=traingen.samples//1)

model.summary()

w_file1 = open("weights-conv1.txt", "w")
w_file2 = open("weights-conv2.txt", "w")
w_file3 = open("weights-conv3.txt", "w")
w_file4 = open("weights-conv4.txt", "w")
w_file5 = open("weights-conv5.txt", "w")
w_filefully = open("weights-fully.txt", "w")
w_file = open("weights-dataset.txt", "w")
first = True

n = 0
fully = 0
string_total = str()

for layer in model.layers:
    weights = layer.get_weights()
    
    if len(weights) is not 0:

        for line in weights:
            for column in line:
                try:
                    for deepness in column:
                        for conv_filter in deepness:
                            for element in conv_filter:
                                if first:
                                    if n == 0:
                                        w_file1.write(str(element))
                                    elif n >= 1 and n <= 4:
                                        w_file2.write(str(element))
                                    elif n >= 5 and n <= 9:
                                        w_file3.write(str(element))
                                    elif n >= 10 and n <=14:
                                        w_file4.write(str(element))
                                    elif n >= 15 and n <= 19:
                                        w_file5.write(str(element))
                                    #elif n == 20:
                                        #w_filefully.write(str(element))
                                        #fully = fully + 1
                                    
                                    w_file.write(str(element))
                                    first = False
                                else:
                                    if n == 0:
                                        w_file1.write("*" + str(element))
                                    elif n >= 1 and n <= 4:
                                        w_file2.write("*" + str(element))
                                    elif n >= 5 and n <= 9:
                                        w_file3.write("*" + str(element))
                                    elif n >= 10 and n <=14:
                                        w_file4.write("*" + str(element))
                                    elif n >= 15 and n <= 19:
                                        w_file5.write("*" + str(element))
                                    #elif n == 20:
                                        #w_filefully.write("*" + str(element))
                                        #fully = fully + 1
                                    
                                    w_file.write("*" + str(element))
                except:
                    for element in column:
                        if first:
                            if n == 0:
                                w_file1.write(str(element))
                            elif n >= 1 and n <= 4:
                                w_file2.write(str(element))
                            elif n >= 5 and n <= 9:
                                w_file3.write(str(element))
                            elif n >= 10 and n <=14:
                                w_file4.write(str(element))
                            elif n >= 15 and n <= 19:
                                w_file5.write(str(element))
                            elif n == 20:
                                #w_filefully.write(str(element))
                                #print(element)
                                string_total = "*" + str(element)
                                fully = fully + 1

                            w_filefully.write(str(element))
                            first = False
                        else:
                            if n == 0:
                                w_file1.write("*" + str(element))
                            elif n >= 1 and n <= 4:
                                w_file2.write("*" + str(element))
                            elif n >= 5 and n <= 9:
                                w_file3.write("*" + str(element))
                            elif n >= 10 and n <=14:
                                w_file4.write("*" + str(element))
                            elif n >= 15 and n <= 19:
                                w_file5.write("*" + str(element))
                            elif n == 20:
                                #w_filefully.write("*" + str(element))
                                #print(element)
                                string_total = string_total + "*" + str(element)
                                fully = fully + 1
                            
                            w_file.write("*" + str(element))
        w_filefully.write(string_total)
        if n == 0:
            w_file1.write("\n")
        elif n >= 1 and n <= 4:
            w_file2.write("\n")
        elif n >= 5 and n <= 9:
            w_file3.write("\n")
        elif n >= 10 and n <= 14:
            w_file4.write("\n")
        elif n >= 15 and n <= 19:
            w_file5.write("\n")
        elif n == 20:
            w_filefully.write("\n")

        w_file.write("\n")
        first = True
        n = n + 1
    
print(fully)

model.save("weights.h5")
out = model.predict(outgen)
print(out)