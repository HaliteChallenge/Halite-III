"""Make usernames/team names actually case insenstive unique

Revision ID: b56f74b8f3e5
Revises: 3afffc53a4b4
Create Date: 2018-10-10 14:16:25.252365+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = 'b56f74b8f3e5'
down_revision = '3afffc53a4b4'
branch_labels = None
depends_on = None


def upgrade():
    op.drop_index("uq_user_username", "user")
    op.drop_index("uq_team_name", "team")
    op.create_index("uq_user_username", "user", [sa.text("lower(username)")], unique=True)
    op.create_index("uq_team_name", "team", [sa.text("lower(name)")], unique=True)


def downgrade():
    op.drop_index("uq_user_username", "user")
    op.drop_index("uq_team_name", "team")
    op.create_index("uq_user_username", "user", [sa.text("lower(username)")])
    op.create_index("uq_team_name", "team", [sa.text("lower(name)")])
