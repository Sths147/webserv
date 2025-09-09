PWD := $(shell pwd)

l :
	docker logs nginx-test


r :
	docker run -d -p 8080:80\
		-v $(PWD)/nginx.conf:/etc/nginx/nginx.conf \
		-v $(PWD)/test2:/usr/share/nginx/serve2/html \
		-v $(PWD):/usr/share/nginx/html \
		--name nginx-test nginx

s :
	docker stop nginx-test  && docker rm nginx-test
