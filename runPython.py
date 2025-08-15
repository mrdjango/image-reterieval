import subprocess

path = ['/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(0,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/1/(1,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(1,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/2/(2,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(2,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/3/(3,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,2).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,3).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,4).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,5).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(3,6).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,1).png',
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,2).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,3).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,4).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,5).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/4/(4,6).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,1).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,2).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,3).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,4).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,5).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(4,6).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,1).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,2).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,3).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,4).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,5).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/5/(5,6).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,1).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,2).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,3).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,4).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,5).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(5,6).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,1).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,2).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,3).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,4).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,5).png', 
		'/home/hossein/CharRecognition/image-reterieval/test_images/6/(6,6).png']

results = []

for p in path:
	
	# Define the command and its arguments as a list
	command = ["/home/hossein/CharRecognition/image-reterieval/src/build_cpu/recognize", p]

	# Execute the command
	result = subprocess.run(command, capture_output=True, text=True)

	# Print the output (stdout)
	#print(result.stdout)
	
	output = result.stdout

	# Find the starting index of the clause
	start_index = output.find('Min distance:')

	# If clause is found, extract everything after it
	if start_index != -1:
		print(p)
		extracted_text = output[start_index + len('Min distance:'):].strip()
		print(extracted_text)
		
		aa = p + '\n' + extracted_text + '\n\n\n'
		results.append(aa)

file1 = open('res.txt', 'w')
file1.writelines(results)
file1.close()
